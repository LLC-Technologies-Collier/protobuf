# Object Caching

_Status: C Layer Implemented_

To ensure object identity and improve performance, the implementation uses a global (per-interpreter) cache to map underlying UPB C objects (pointers) to their corresponding Perl wrapper SVs. This mechanism is critical for maintaining consistent Perl object identity and efficient memory management.

## Cache Mechanism

The C implementation of the object cache is located in `perl/xs/protobuf/obj_cache.c` and `perl/xs/protobuf/obj_cache.h`.

*   **Global Cache:** A single `HV*` (Perl Hash) is initialized during module load.
*   **Keys:** Hexadecimal string representations of C pointer addresses (e.g., `0x7fd1a2b3c4d5`).
*   **Values:** Weak references (using `sv_rvweaken`) to the blessed Perl objects. This ensures that the cache itself does not extend the lifetime of the Perl wrappers.

## API Functions

-   `void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj)`: Adds a Perl object to the cache for the given C pointer. The reference in the cache is weakened.
-   `SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr)`: Retrieves the Perl object associated with the C pointer. Returns `NULL` if not found or if the weak reference has been collected.
-   `void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr)`: Removes the entry for the given C pointer from the cache.

## Usage in XS

Any function that needs to return a Perl wrapper for a `upb` object MUST:

1.  Call `PerlUpb_ObjCache_Get(aTHX_ ptr)`.
2.  If it returns a valid SV, increment its reference count and return it.
3.  If it returns `NULL`, create the new Perl wrapper SV, call `PerlUpb_ObjCache_Add(aTHX_ ptr, new_sv)`, and then return the new SV.

## Benefits

-   **Object Identity:** Ensures that multiple calls for the same underlying C descriptor or message return the same Perl object instance.
-   **Memory Efficiency:** Prevents redundant Perl wrapper objects from being created for long-lived C objects (like descriptors in the `DescriptorPool`).
-   **Automatic Cleanup:** Using weak references allows Perl's garbage collector to reclaim the wrapper objects when they are no longer in use by the Perl application, at which point the cache entry effectively becomes empty.
