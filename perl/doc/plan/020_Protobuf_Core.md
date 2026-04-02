# Milestone 2: C Layer - protobuf (Core Utilities)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/protobuf/obj_cache.c:**
    *   [x] Create test file `perl/t/c/protobuf/obj_cache.c`.
    *   [x] Tests in `perl/t/c/protobuf/obj_cache.c` test all public functions in `perl/xs/protobuf/obj_cache.h`.
    *   [x] TODO: Performance benchmarking for large caches (O(1) lookups).
    *   [x] TODO: Concurrency integrity during interleaved GC (Coro/Mojo).
    *   [x] TODO: Implement **Lock-Free Cache Operations:** Ensure that object retrieval and caching are thread-safe without heavy global locks.
    *   [x] TODO: Implement **interpreter-isolation verification:** Verify that cache state is strictly isolated between multiple Perl interpreters (ithreads).
    *   [x] TODO: Implement LRU eviction or memory-pressure based clearing for the object cache.
    *   [ ] TODO: Implement a high-performance trace/audit log for cache hits/misses to aid in memory leak hunting.
        *   [x] Implement per-interpreter circular ring buffer in C (`obj_cache.c`).
        *   [x] Export `get_cache_audit_log` and `clear_cache` to `Protobuf::Internal`.
        *   [x] Verify `ADD`, `HIT`, and `MISS` event logging.
        *   [ ] TODO: Investigate why `DELETE` events are not reliably triggered for `DescriptorPool` in audit tests (Test 5 in `t/03-obj-cache-audit.t`).
        *   [ ] TODO: Implement reliable eviction triggering to verify `EVICT` event logging (Test 6 in `t/03-obj-cache-audit.t`).
    *   [x] Implement C functions in `perl/xs/protobuf/obj_cache.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/obj_cache.c`.

*   **perl/xs/protobuf/arena.c:**
    *   [x] Create test file `perl/t/c/protobuf/arena.c`.
    *   [x] Tests in `perl/t/c/protobuf/arena.c` test all public functions in `perl/xs/protobuf/arena.h`.
    *   [x] TODO: Memory usage statistics (Allocated).
    *   [ ] TODO: tmpfs-backed custom allocators for zero-copy IPC.
    *   [ ] TODO: Implement thread-local arena caching for ultra-high-frequency small message allocations.
    *   [ ] TODO: Add memory corruption guards (canary bytes) around arena-allocated blocks to detect out-of-bounds writes in C.
    *   [x] Implement C functions in `perl/xs/protobuf/arena.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/arena.c`.

*   **perl/xs/protobuf/utils.c:**
    *   [x] Create test file `perl/t/c/protobuf/utils.c`.
    *   [x] Tests in `perl/t/c/protobuf/utils.c` test all public functions in `perl/xs/protobuf/utils.h`.
    *   [ ] TODO: SIMD acceleration for name conversion/string validation.
    *   [x] TODO: Context-aware croak messages (field path resolution).
    *   [x] Implement C functions in `perl/xs/protobuf/utils.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/utils.c`.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.

