# Milestone 10: C Layer - descriptor_pool

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_pool/pool.c:**
    *   [x] Create test file `perl/t/c/descriptor_pool/pool.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] Implement read-only locks for shared pool access. (Difficulty: 2/10).
    *   [ ] Add singleton verification across multiple interpreters. (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_pool/add.c:**
    *   [x] Create test file `perl/t/c/descriptor_pool/add.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] Implement lazy-loading infrastructure for serialized blobs. (Difficulty: 3/10).
    *   [ ] Add deferred field resolution for lazy-loaded pools. (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_pool/find.c:**
    *   [x] Create test file `perl/t/c/descriptor_pool/find.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] Implement detailed conflict error metadata. (Difficulty: 2/10).
    *   [ ] Add source-file and line-number tracking to conflicts. (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement shared memory pool attachment logic. (Difficulty: 3/10).
    *   [ ] Add cross-process fingerprinting for pool identity. (Difficulty: 3/10).
    *   [ ] Implement sub-second startup benchmarks for large pools. (Difficulty: 3/10).
    *   [ ] Add on-demand parsing hook for missing symbols. (Difficulty: 3/10).
    *   [ ] Implement conflict resolution traceback utility. (Difficulty: 3/10).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
