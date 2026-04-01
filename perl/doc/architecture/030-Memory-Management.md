# Architecture: Memory Management

[TOC]

## C Layer (upb)

1.  **Arenas:** The `upb` library's arena allocation model will be used extensively to manage the lifetime of messages, descriptors, and related C structures.
2.  **Ownership:** C structures created and returned to Perl will typically be owned by the arena associated with the parent Perl object (e.g., a Message object's arena).

## Advanced Memory Management

1.  **Thread-Local Arena Caching**: (Planned) To optimize ultra-high-frequency allocations of small messages, the implementation will support thread-local arena caching, bypassing global locks or complex state checks.
2.  **Zero-Copy IPC**: (Planned) Support for tmpfs-backed custom allocators will allow `upb_Arena` to reside in shared memory segments for zero-copy high-performance inter-process communication.
3.  **Safety Guards**: (Planned) Add memory corruption guards (canary bytes) around arena-allocated blocks to detect out-of-bounds writes in the C layer during development and automated testing.
4.  **Observability**: (Planned) Observation APIs to report detailed arena memory usage statistics (Allocated vs. Reserved) to aid in capacity planning and leak detection.
