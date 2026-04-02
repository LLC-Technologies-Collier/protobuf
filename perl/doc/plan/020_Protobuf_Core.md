# Milestone 2: C Layer - protobuf (Core Utilities)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/protobuf/obj_cache.c:**
    *   [x] Create test file `perl/t/c/protobuf/obj_cache.c`.
    *   [x] Tests in `perl/t/c/protobuf/obj_cache.c` test all public functions in `perl/xs/protobuf/obj_cache.h`.
    *   [x] TODO: Performance benchmarking for large caches (O(1) lookups).
    *   [x] TODO: Concurrency integrity during interleaved GC (Coro/Mojo).
    *   [x] Implement **Lock-Free Cache Operations:** Ensure that object retrieval and caching are thread-safe using striped mutexes.
    *   [x] Implement **interpreter-isolation verification:** Verify that cache state is strictly isolated between multiple Perl interpreters (ithreads).
    *   [x] Implement LRU eviction for the object cache.
    *   [ ] TODO: Implement a high-performance trace/audit log for cache hits/misses to aid in memory leak hunting.
        *   [x] Implement per-interpreter circular ring buffer in C (`obj_cache.c`).
        *   [x] Export `get_cache_audit_log` and `clear_cache` to `Protobuf::Internal`.
        *   [x] Verify `ADD`, `HIT`, and `MISS` event logging.
        *   [ ] TODO: Investigate why `DELETE` events are not reliably triggered for `DescriptorPool` in audit tests (Test 5 in `t/03-obj-cache-audit.t`).
        *   [ ] TODO: Implement reliable eviction triggering to verify `EVICT` event logging (Test 6 in `t/03-obj-cache-audit.t`).
    *   [x] Implement C functions in `perl/xs/protobuf/obj_cache.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/obj_cache.c`.

*   **perl/xs/protobuf/registry.c (NEW):**
    *   [x] Implement **Per-Interpreter Registry Pattern** (Centralized State).
    *   [x] Create a single C struct to hold all interpreter-local state (Object Cache, Audit Log, LRU).
    *   [x] Integrate with Perl's `PL_modglobal` for ultra-fast, lock-free state retrieval within an interpreter.
    *   [x] Refactor `obj_cache.c` to eliminate expensive global SV lookups.

*   **perl/xs/protobuf/arena.c:**
    *   [x] Create test file `perl/t/c/protobuf/arena.c`.
    *   [x] Tests in `perl/t/c/protobuf/arena.c` test all public functions in `perl/xs/protobuf/arena.h`.
    *   [x] Implement Memory usage statistics (Allocated vs Reserved).
    *   [x] Implement tmpfs-backed custom allocators for zero-copy IPC.
    *   [x] Implement **Arena Factory Pattern** (`PerlUpb_Arena_Acquire`).
        *   [x] Support lifecycle hints: `PERL_UPB_LIFECYCLE_PERMANENT` (standard) vs `PERL_UPB_LIFECYCLE_TRANSIENT` (fast-path).
        *   [x] Abstract away the distinction between fresh arenas and cached arenas from the rest of the C layer.
    *   [x] Implement thread-local arena caching for ultra-high-frequency small message allocations.
    *   [x] Add memory corruption guards (canary bytes) around arena-allocated blocks to detect out-of-bounds writes in C.
    *   [x] Implement C functions in `perl/xs/protobuf/arena.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/arena.c`.

*   **perl/xs/protobuf/arena_tmpfs.c:**
    *   [x] Implement tmpfs-backed custom allocator.
    *   [x] Implement **Block Allocator Pattern** (Generalized `upb_alloc`).
        *   [x] Refactor `arena_tmpfs.c` to support both `mmap` (file-backed) and `malloc` (RAM-backed) linear allocation buffers.
        *   [x] Enable reuse of the linear allocator for the thread-local fast-path arena.

*   **perl/xs/protobuf/utils.c:**
    *   [x] Create test file `perl/t/c/protobuf/utils.c`.
    *   [x] Tests in `perl/t/c/protobuf/utils.c` test all public functions in `perl/xs/protobuf/utils.h`.
    *   [x] Implement SIMD acceleration for name conversion using SSE4.1.
    *   [x] TODO: Context-aware croak messages (field path resolution).
    *   [x] Implement C functions in `perl/xs/protobuf/utils.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/utils.c`.

*   **Engineering Excellence (Reach for More):**
    *   [x] Implement O(1) Descriptor lookup by fingerprinted hash for ultra-fast dispatch.
    *   [ ] TODO: Implement NUMA-aware arena allocation to optimize multi-socket memory placement. (Difficulty: 9/10).
    *   [x] Implement Chaos Allocation Engine for non-deterministic resource pressure testing.
    *   [x] Implement Audit-log driven leak sanitizer for sub-arena allocation tracking.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.

