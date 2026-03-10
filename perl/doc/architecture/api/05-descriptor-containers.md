# Descriptor Containers

_Status: Not Started_

To efficiently expose collections of descriptors (like fields, nested messages, enum values) from a parent descriptor, we will implement generic container types in XS, similar to Python's `descriptor_containers.c`. This avoids creating many unique Perl classes for each type of collection.

## Design

Instead of returning Perl arrays or hashes directly from methods like `$descriptor->fields`, we will return blessed objects representing these collections. These objects will be lightweight and fetch information on demand from the underlying `upb` definitions.

*   **Generic XS Containers:** Create a few generic XS container types, for example:
    *   `Protobuf::Internals::DescriptorSequence`: For ordered lists like `fields`, `enum_types`.
    *   `Protobuf::Internals::DescriptorByNameMap`: For string-keyed mappings like `fields_by_name`.
    *   `Protobuf::Internals::DescriptorByNumberMap`: For integer-keyed mappings like `fields_by_number`.

*   **VTable/Funcs Struct:** Each instance of these generic containers will hold:
    *   A pointer to the parent `upb` definition (e.g., `const upb_MessageDef*`).
    *   A reference to the parent Perl object to ensure it stays alive.
    *   A struct of function pointers (a "vtable") that knows how to:
        *   Get the count of elements.
        *   Get an element by index.
        *   Get an element by name (for maps).
        *   Get an element by number (for maps).
        *   Get the name/number for an element (for map keys).
        *   Wrap the raw `upb` pointer in the appropriate Perl object (using the object cache).

*   **Perl Interface:** These XS objects will be blessed into Perl classes that overload array/hash operations to provide an idiomatic Perl interface, but the actual data access will be delegated to the C functions in the vtable.

## Example (Conceptual XS)

```c
// In a Descriptor.xs file
SV* fields(Protobuf__Descriptor self) {
    const upb_MessageDef* mdef = (const upb_MessageDef*)self->_upb_def;
    // vtable for fields
    static const Protobuf__Internals__DescriptorSequence_Funcs field_funcs = {
        upb_MessageDef_FieldCount,
        upb_MessageDef_Field,
        wrap_field_descriptor, // XS function to get/create Protobuf::FieldDescriptor wrapper
    };
    return create_descriptor_sequence(aTHX_ self, mdef, &field_funcs);
}
```

## Benefits

*   **Efficiency:** Avoids creating large Perl arrays/hashes upfront. Objects are wrapped only when accessed.
*   **Reduced Code:** Reuses the same generic container logic for many different descriptor collections.
*   **Consistency:** Aligns with the approach taken by the Python UPB extension.
