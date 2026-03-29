# Descriptor Containers

_Status: C Layer Implemented_

To efficiently expose collections of descriptors (like fields, nested messages, enum values) from a parent descriptor, we use generic container types in XS. This avoids creating many unique Perl classes for each type of collection.

## Implementation

The C implementation of the descriptor containers is located in `perl/xs/descriptor_containers/`.

*   **`PerlUpb_ByNameMap` (`by_name_map.[ch]`):** A lazy Perl map wrapper for string-keyed upb collections. It uses a vtable to delegate operations to the underlying upb object.
*   **`PerlUpb_ByNumberMap` (`by_number_map.[ch]`):** A lazy Perl map wrapper for integer-keyed upb collections.
*   **`PerlUpb_GenericSequence` (`generic_sequence.[ch]`):** A lazy Perl array-like wrapper for upb collections.
*   **`PerlUpb_MapIterator` (`iterators.[ch]`):** Provides iteration support for the map containers.

### VTable Pattern

Each container type uses a vtable (a struct of function pointers) to interact with the parent upb object. This allows the same container logic to be reused for different types of descriptors (e.g., fields of a message, values of an enum).

Example `PerlUpb_ByNameMap_VTable`:
- `count`: Returns the number of items.
- `lookup`: Looks up an item by name.
- `key`: Returns the string name for an item at a given index.
- `value`: Returns the upb item at a given index.
- `wrap`: An implementation-specific function that takes the raw `upb_Def*` and returns the appropriate blessed Perl object (using the global object cache).

## Perl Interface

These XS objects are blessed into internal Perl classes:
- `Protobuf::Internals::DescriptorByNameMap`
- `Protobuf::Internals::DescriptorByNumberMap`
- `Protobuf::Internals::DescriptorSequence`

The Perl layer (to be implemented in Moo classes) will wrap these internal objects and use operator overloading (e.g., `Tie::Hash`, `Tie::Array`, or `overload`) to provide an idiomatic Perl experience.

## Benefits

*   **Efficiency:** Avoids creating large Perl arrays/hashes upfront. C objects are wrapped in Perl only when they are actually accessed.
*   **Reduced Code:** Reuses the same generic container logic for many different descriptor collections.
*   **Memory Management:** The container objects hold a strong reference to the parent Perl object (e.g., the `MessageDescriptor`), ensuring the underlying upb definitions stay alive as long as the container is in use.
