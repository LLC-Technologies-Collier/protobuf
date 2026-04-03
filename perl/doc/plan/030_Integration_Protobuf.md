# Milestone 3: C Layer - Integration Tests (protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/030_protobuf.c`.
*   [x] Tests cover interactions between obj_cache, arena, and utils.
*   [x] Verify basic arena-sharing integrity.
*   [x] Add per-interpreter registry validation tests.
*   [x] Verify cache isolation across 10 independent interpreters.
*   [x] Implement safety checks for `croak()` during upb_Arena growth.
*   [x] Verify state integrity after simulated longjmp.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/030_protobuf_coro.c` using libcoro.
*   [x] Tests in `perl/t/c/integration/030_protobuf_coro.c` stress concurrent C function usage.
*   [ ] Implement lock contention profiling for striped cache mutexes. (Difficulty: 3/10).
*   [ ] Verify progress during high-frequency concurrent lookups. (Difficulty: 3/10).
*   [ ] Add assembly-level TSAN annotations to Registry hot-paths. (Difficulty: 3/10).
*   [ ] Implement automated race-detection in Registry CI. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement shared memory cache attachment logic. (Difficulty: 3/10).
    *   [ ] Add cross-process fingerprinting for object identity. (Difficulty: 3/10).
    *   [ ] Implement idle-time arena pre-allocation hook. (Difficulty: 3/10).
    *   [ ] Add predictive allocator based on historical usage. (Difficulty: 3/10).
    *   [ ] Implement canary-based block verification in `upb_alloc` overrides. (Difficulty: 3/10).
    *   [ ] Add automated recovery logic for corrupted arena blocks. (Difficulty: 3/10).
