# Milestone 19: C Layer - Integration Tests (repeated + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/190_repeated.c`.
*   [x] Tests cover repeated field interactions with messages. (Verified scalar and message array roundtrips).
*   [ ] TODO: Implement high-performance cross-arena repeated field deep-copy. (Difficulty: 6/10).
    *   [ ] TODO: Implement basic cross-arena deep-copy for scalar repeated fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement recursive deep-copy for message repeated fields across arenas. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark and optimize memory allocation during bulk deep-copy operations. (Difficulty: 3/10).
*   [ ] TODO: Implement SIMD-accelerated array scanning (find/grep) utilities. (Difficulty: 8/10).
    *   [ ] TODO: Research SIMD instructions for fixed-width integer array scanning (e.g., AVX2). (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized 'find' operation for numeric repeated fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized 'grep' (filtering) operation for numeric repeated fields. (Difficulty: 3/10).
*   [ ] TODO: Implement deterministic unique-set merging for repeated fields. (Difficulty: 5/10).
*   [x] Integration tests pass. (Fixed leaks and verified sub-message arrays).
*   [x] Create test file `perl/t/c/integration/190_repeated_coro.c`
*   [ ] Tests in `perl/t/c/integration/190_repeated_coro.c` use libcoro to stress concurrent C function usage for repeated. (Partially implemented).
*   [ ] TODO: Stress concurrent repeated field expansion under memory pressure. (Difficulty: 7/10).
    *   [ ] TODO: Implement concurrent array expansion test using libcoro. (Difficulty: 3/10).
    *   [ ] TODO: Implement artificial memory pressure simulation during expansion tests. (Difficulty: 3/10).
    *   [ ] TODO: Verify graceful handling of out-of-memory conditions during concurrent expansion. (Difficulty: 3/10).
*   [ ] TODO: Verify integrated cache stability during concurrent sub-message array mutation. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent sub-message addition and cache invalidation test. (Difficulty: 3/10).
    *   [ ] TODO: Verify cache consistency when sub-messages are mutated concurrently by multiple threads. (Difficulty: 3/10).
    *   [ ] TODO: Implement stress test for concurrent cache access and refresh. (Difficulty: 3/10).
*   [ ] TODO: Implement race detection for shared arena array access. (Difficulty: 9/10).
    *   [ ] TODO: Research and integrate ThreadSanitizer or similar tool for C-level race detection. (Difficulty: 3/10).
    *   [ ] TODO: Implement instrumentation for tracking concurrent access to shared arena arrays. (Difficulty: 3/10).
    *   [ ] TODO: Implement automated tests that trigger and detect known race conditions in array access. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Lazy Repeated Entry Inflation for deferred wrapper creation. (Difficulty: 7/10).
        *   [ ] TODO: Design proxy mechanism for deferred inflation of repeated message entries. (Difficulty: 3/10).
        *   [ ] TODO: Implement lazy wrapper creation on first access to a repeated message field element. (Difficulty: 3/10).
        *   [ ] TODO: Verify memory reduction and performance impact for sparse access patterns. (Difficulty: 3/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Aggregation utilities. (Difficulty: 8/10).
        *   [ ] TODO: Research SSE4.2 aggregation primitives (e.g., horizontal sums). (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized sum and average operations for numeric repeated fields. (Difficulty: 3/10).
        *   [ ] TODO: Verify correctness and benchmark against scalar aggregation implementations. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Repeated Array Consistency background auditing. (Difficulty: 8/10).
        *   [ ] TODO: Implement background auditor for repeated field array structures. (Difficulty: 3/10).
        *   [ ] TODO: Implement detection and repair for array length mismatches or invalid pointers. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor's ability to recover from simulated array corruption without crashing. (Difficulty: 3/10).

