# Descriptor Pool Management

_Status: C Layer Implemented_

The `Protobuf::DescriptorPool` class manages a collection of descriptors. It is a wrapper around `upb_DefPool`.

## Perl Level (`Protobuf::DescriptorPool`)

-   The Perl object is a Moo class.
-   It contains an opaque pointer to the `upb_DefPool*` (via the `_pool_ptr` attribute).
-   It provides methods to load serialized `FileDescriptorProto` data (`add_serialized_file`).
-   It has methods to find message, enum, or extension definitions by their fully qualified name (`find_message_by_name`, etc.).

## XS Level (`DescriptorPool.xs`)

-   **`_xs_create_raw`**: Creates a new `upb_DefPool` and wraps it.
-   **`_xs_destroy_raw`**: Frees the `upb_DefPool` when the Perl object is destroyed.
-   **`_xs_add_serialized_file`**: Parses serialized descriptor data and adds it to the pool.
-   **Search methods**: Provide direct access to `upb_DefPool_Find*` functions.

## Implementation Details

-   The `upb_DefPool` owns its own memory for descriptors.
-   The object cache ensures that multiple accesses to the same descriptor return the same Perl wrapper.

## Lifecycle

-   The `upb_DefPool` is owned by the `Protobuf::DescriptorPool` Perl object and freed in `DEMOLISH`.
-   Descriptors returned from the pool hold weak or cached references; the pool must remain alive as long as its descriptors are in use.
