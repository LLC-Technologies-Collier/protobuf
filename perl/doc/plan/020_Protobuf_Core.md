# Milestone 2: C Layer - protobuf (Core Utilities)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/protobuf/obj_cache.c:**
    *   [x] Create test file `perl/t/c/protobuf/obj_cache.c`.
    *   [ ] Tests in `perl/t/c/protobuf/obj_cache.c` test all public functions in `perl/xs/protobuf/obj_cache.h`. (Partially implemented).
    *   [ ] TODO: Performance benchmarking for large caches (O(1) lookups).
    *   [ ] TODO: Concurrency integrity during interleaved GC (Coro/Mojo).
    *   [x] Implement C functions in `perl/xs/protobuf/obj_cache.c`.
    *   [ ] C tests pass for `perl/t/c/protobuf/obj_cache.c`.

*   **perl/xs/protobuf/arena.c:**
    *   [x] Create test file `perl/t/c/protobuf/arena.c`.
    *   [ ] Tests in `perl/t/c/protobuf/arena.c` test all public functions in `perl/xs/protobuf/arena.h`. (Partially implemented).
    *   [ ] TODO: Memory usage statistics (Allocated vs. Reserved).
    *   [ ] TODO: tmpfs-backed custom allocators for zero-copy IPC.
    *   [x] Implement C functions in `perl/xs/protobuf/arena.c`.
    *   [ ] C tests pass for `perl/t/c/protobuf/arena.c`.

*   **perl/xs/protobuf/utils.c:**
    *   [x] Create test file `perl/t/c/protobuf/utils.c`.
    *   [ ] Tests in `perl/t/c/protobuf/utils.c` test all public functions in `perl/xs/protobuf/utils.h`. (Partially implemented).
    *   [ ] TODO: SIMD acceleration for name conversion/string validation.
    *   [ ] TODO: Context-aware croak messages (field path resolution).
    *   [x] Implement C functions in `perl/xs/protobuf/utils.c`.
    *   [ ] C tests pass for `perl/t/c/protobuf/utils.c`.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.

