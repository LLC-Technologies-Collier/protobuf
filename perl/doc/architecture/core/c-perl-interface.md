# C-Perl Interface (XS)

The interface between Perl and the C `upb` library will be implemented using XS.

## Key Principles

1.  **Thin Wrappers:** Perl objects (e.g., `Protobuf::Descriptor`, `Protobuf::Message`) will be thin wrappers around the C `upb` structs (`upb_MessageDef`, `upb_Message`). The Perl object will primarily hold a pointer to the C struct.

2.  **Object Ownership:** The `upb` C objects are arena-allocated. The Perl wrapper objects do not *own* the C objects in terms of memory management. The `Protobuf::Arena` object wrapper will be responsible for freeing the underlying `upb_Arena`.

3.  **Typemaps:** Custom typemaps will be used to convert between C pointers and Perl objects. These typemaps will leverage the Object Caching mechanism to ensure the same Perl object is returned for the same C pointer.

    -   `T_PTROBJ_UPB`: A typemap entry for `upb` pointers that need to be cached and wrapped.

4.  **XS Functions:** XSUBs will:
    -   Accept Perl objects as arguments.
    -   Extract the underlying C pointer from the Perl object.
    -   Call the appropriate `upb` library functions.
    -   Wrap the returned `upb` pointers (if any) back into Perl objects using the caching mechanism.

## Example Workflow (Conceptual)

```c
// XS function to get a message's descriptor
SV* get_descriptor(pTHX_ Protobuf__Message self) {
    upb_Message *msg = INT2PTR(upb_Message*, SvIV(SvRV(self)));
    const upb_MessageDef *mdef = upb_Message_GetDef(msg);

    // This function will handle cache lookup or creation
    return protobuf_c_to_perl_obj(aTHX_ mdef, "Protobuf::Descriptor");
}
```

## Integration and Stability

To ensure world-class performance and stability in integrated environments (e.g., Mojo, Coro), the interface follows these advanced principles:

1.  **Arena Sharing and Pinning**: When returning a sub-message or descriptor, the XS layer MUST ensure the child object pins the parent's memory by holding a strong reference to the parent's `arena_sv` or `pool_sv`. This ensures that the underlying `upb_Arena` remains valid even if the parent Perl object goes out of scope.
2.  **Cross-Interpreter Isolation**: Every `PerlInterpreter` instance (ithreads or workers) MUST have its own independent XS state. The object cache and any internal C-level registries are stored as member variables of the interpreter context (or via `PL_modglobal`) to prevent data corruption between threads.
3.  **Interrupt Resilience (Croak Safety)**: The interface is designed to maintain state integrity if a Perl `croak` or `die` (which uses `longjmp`) occurs during an operation. Resource management in C follows the "Allocate-Then-Register" pattern to ensure that any partially created objects are either properly tracked or safely leaked only until the arena is destroyed.
4.  **Lock-Free Multi-Access**: Read-only operations (like descriptor lookups) are architected to be lock-free, enabling high-performance concurrent access from multiple Perl coroutines.
