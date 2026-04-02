# Milestone 19: C Layer - Integration Tests (repeated + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/190_repeated.c`.
*   [x] Tests cover repeated field interactions with messages. (Verified scalar and message array roundtrips).
*   [ ] TODO: Implement high-performance cross-arena repeated field deep-copy. (Difficulty: 6/10).
*   [ ] TODO: Implement SIMD-accelerated array scanning (find/grep) utilities. (Difficulty: 8/10).
*   [ ] TODO: Implement deterministic unique-set merging for repeated fields. (Difficulty: 5/10).
*   [x] Integration tests pass. (Fixed leaks and verified sub-message arrays).
*   [x] Create test file `perl/t/c/integration/190_repeated_coro.c`
*   [ ] Tests in `perl/t/c/integration/190_repeated_coro.c` use libcoro to stress concurrent C function usage for repeated. (Partially implemented).
*   [ ] TODO: Stress concurrent repeated field expansion under memory pressure. (Difficulty: 7/10).
*   [ ] TODO: Verify integrated cache stability during concurrent sub-message array mutation. (Difficulty: 6/10).
*   [ ] TODO: Implement race detection for shared arena array access. (Difficulty: 9/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Lazy Repeated Entry Inflation for deferred wrapper creation. (Difficulty: 7/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Aggregation utilities. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing Repeated Array Consistency background auditing. (Difficulty: 8/10).

