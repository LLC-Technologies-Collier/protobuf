# Milestone 3: C Layer - Integration Tests (protobuf)

[TOC]

*   [x] Add per-interpreter registry validation tests.
*   [x] Verify cache isolation across 10 independent interpreters.
*   [x] Implement safety checks for `croak()` during upb_Arena growth.
*   [x] Verify state integrity after simulated longjmp.
*   [x] Create test file `perl/t/c/integration/030_protobuf.c`.
*   [x] Tests cover interactions between obj_cache, arena, and utils.
*   [x] Verify basic arena-sharing integrity.
*   [x] Implement lock contention profiling for striped cache mutexes. (Difficulty: 3/10).
*   [x] Verify progress during high-frequency concurrent lookups. (Difficulty: 3/10).
*   [ ] Add assembly-level TSAN annotations to Registry hot-paths. (Difficulty: 3/10).
*   [x] Implement automated race-detection in Registry CI. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [x] Implement shared memory cache attachment logic. (Difficulty: 3/10).
    *   [x] Add cross-process fingerprinting for object identity. (Difficulty: 3/10).
    *   [x] Implement idle-time arena pre-allocation hook. (Difficulty: 3/10).
    *   [x] Add predictive allocator based on historical usage. (Difficulty: 3/10).
    *   [x] Implement canary-based block verification in `upb_alloc` overrides. (Difficulty: 3/10).
    *   [ ] Add automated recovery logic for corrupted arena blocks. (Difficulty: 3/10).
    *   [ ] NUMA-Aware Allocation Balance. (Difficulty: 5/10).
    *   [ ] COW-Optimized Shared Cache. (Difficulty: 7/10).
    *   [ ] SIMD-Accelerated Integrity Scanning. (Difficulty: 6/10).
