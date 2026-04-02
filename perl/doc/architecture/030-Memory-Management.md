# Architecture: Memory Management

[TOC]

## C Layer (upb)

1.  **Arenas:** The `upb` library's arena allocation model will be used extensively to manage the lifetime of messages, descriptors, and related C structures.
2.  **Ownership:** C structures created and returned to Perl will typically be owned by the arena associated with the parent Perl object (e.g., a Message object's arena).

## Advanced Memory Management

1.  **Thread-Local Arena Caching (VPP Pattern)**: (Planned) To optimize ultra-high-frequency allocations of small messages, the implementation will support thread-local arena caching. Inspired by the **Vector Packet Processor (VPP)** architecture, this pattern keeps a "warm" arena per interpreter to ensure that message memory resides in the L1/L2 data cache.
2.  **Vectorized Processing**: (Planned) Implement a "Batch Parse" API that ingest a vector of incoming binary blobs. By processing messages in batches through a single graph node (parsing logic), we minimize instruction cache (I-cache) thrashing and maximize hardware pre-fetcher efficiency.
3.  **Zero-Copy IPC**: (Planned) Support for tmpfs-backed custom allocators will allow `upb_Arena` to reside in shared memory segments for zero-copy high-performance inter-process communication, mimicking VPP's zero-copy packet handoff between graph nodes.
3.  **Safety Guards**: (Implemented) Memory corruption guards (canary bytes) are placed around arena-allocated blocks. A 16-byte pattern (`0xDEADBEEFCAFEBABEULL`) is written to the start and end of every block. These canaries are verified during `free` and `realloc` operations, and upon arena destruction, to detect out-of-bounds writes.
4.  **Observability**: (Implemented) Observation APIs report detailed arena memory usage statistics (Allocated, Reserved, and Block count). See `PerlUpb_Arena_GetStats`.
5.  **Merging Efficiency**: (Implemented) High-performance message merging (`parse_from`) is architected to utilize arena sharing, allowing data from one message tree to be integrated into another with minimal new allocations. Verified basic arena-sharing integrity in C integration tests.
6.  **Arena Fusion**: (Planned) Implement a high-performance "fusion" operation at the Perl level, allowing two independent arenas to be merged into a single logical lifecycle to support efficient message tree migration.
7.  **Custom Allocators**: (Planned) Extend `Protobuf::Arena` to support specialized allocator backends, including fixed-size pre-allocated pools for real-time systems.
