# DescriptorPool Management

_Status: Not Started_

This document outlines how `Protobuf::DescriptorPool` in Perl interfaces with the `upb_DefPool` in the C layer.

## Perl Level (`Protobuf::DescriptorPool`)

-   The Perl object is a Moo class.
-   It contains an opaque pointer to the `upb_DefPool*` (via the `_upb_defpool` attribute).
-   It also owns a `upb_Arena*` (via the `_upb_arena` attribute) used for parsing loaded descriptor sets and storing the definitions, ensuring the parsed data lives as long as the pool.
-   **Descriptor Cache:** Owns a hash (`_descriptor_cache`) to cache `Protobuf::Descriptor` objects, keyed by the C `upb_Def*` pointers.
-   It provides methods to load serialized `FileDescriptorSet` protos (`load_serialized_descriptor_set`).
-   It has methods to find message, enum, or extension definitions by their fully qualified name (`find_message_by_name`, etc.).

## XS Level (`DescriptorPool.xs`)

-   **`pb_dp_create_pool` / `pb_dp_create_arena`:** Called by Moo builders to create the C-level `upb_DefPool` and `upb_Arena`.
-   **`pb_dp_free_pool` / `pb_dp_free_arena`:** Called by `DEMOLISH` to free the C-level objects. The arena free will also release all definitions.
-   **`_load_serialized_descriptor_set`:** Internal XSUB. Takes the pool object, arena pointer, and serialized data. Parses the data using the pool's arena and adds definitions to the `upb_DefPool` using `upb_DefPool_AddFile`. Handles errors using `upb_Status`.
-   **`pb_dp_find_message_by_name`:** Takes the pool SV and a name string, looks up the `upb_MessageDef` using `upb_DefPool_FindMessageByName`. Uses the object cache to return an existing `Protobuf::Descriptor` SV or creates a new one.

## Comparison with Python Implementation

The Python C++ extension utilizes a `DescriptorDatabase` abstraction, which can store serialized `FileDescriptorProto` messages and parse them on demand. The `DescriptorPool` then caches the inflated objects. This can be more memory-efficient for very large descriptor sets where not all messages are used.

The current Perl design directly uses the `upb_DefPool`, which parses and stores the full descriptor definitions in memory on a `upb_Arena` owned by the `Protobuf::DescriptorPool` object. This is simpler and leverages `upb`'s memory-efficient representation, but means all loaded descriptors reside in memory. We do not currently implement a separate serialized descriptor database.

## Lifecycle

-   The `upb_DefPool` and its arena are owned by the `Protobuf::DescriptorPool` Perl object and freed in `DEMOLISH`.
-   All `upb_MessageDef*` and other definition pointers are owned by the `upb_DefPool`'s arena.
-   `Protobuf::Descriptor` objects hold non-owning pointers to the C definitions but maintain the lifecycle of the owning `Protobuf::DescriptorPool` by holding a strong Perl reference to it within their `_pool` attribute.