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

## Performance and Robustness

-   **Lazy Loading**: (Planned) To optimize memory usage for large schema registries, the `DescriptorPool` will support lazy parsing of `FileDescriptorSet` data. Individual files within the set are only fully parsed into `upb_FileDef` structures when a definition within them is first accessed.
-   **Conflict Resolution**: (Planned) The pool implementation provides detailed error reporting during `add_serialized_file` operations. In the event of a naming conflict, the error message will include precise information about the conflicting files and line numbers (if available) to facilitate debugging in complex multi-source environments.

## Concurrency and Isolation

-   **Interpreter Isolation**: Each `PerlInterpreter` (e.g. in ithreads or different worker processes) normally maintains its own distinct `upb_DefPool` and object cache state to avoid race conditions.
-   **Global Pool Sharing**: (Planned) To optimize memory usage in large-scale deployments (e.g. dozens of worker processes), the architecture includes support for a **shared, read-only global pool**. Once a pool is "frozen" (no more additions allowed), it can be safely mapped into multiple interpreters, sharing the underlying C definitions while maintaining per-interpreter Perl wrappers in the object cache.
-   **Lock-Free Retrieval**: High-performance implementations should aim for lock-free or highly-concurrent descriptor retrieval to avoid bottlenecking concurrent message parsing (e.g. in Mojo or Coro).
-   **Dynamic Descriptor Reloading**: (Planned) Implement support for updating existing descriptor definitions in the pool without invalidating active message instances, supporting high-availability protocol upgrades.
-   **Reflection Cache**: (Planned) To minimize XS overhead for frequent reflection tasks, the `DescriptorPool` will maintain a Perl-level cache of message and field definitions.
-   **Symbol Dependency Visualization**: (Planned) Provide a `graph()` utility to export the full symbol dependency tree in DOT or JSON format for architectural analysis of large protocol registries.

## Lifecycle

-   The `upb_DefPool` is owned by the `Protobuf::DescriptorPool` Perl object and freed in `DEMOLISH`.
-   Descriptors returned from the pool hold weak or cached references; the pool must remain alive as long as its descriptors are in use.
