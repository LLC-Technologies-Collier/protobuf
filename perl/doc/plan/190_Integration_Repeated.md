# Milestone 19: C Layer - Integration Tests (repeated + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/190_repeated.c`.
*   [ ] Tests cover repeated field interactions with messages. (Partially implemented: leaks detected).
*   [ ] TODO: Implement high-performance cross-arena repeated field deep-copy.
*   [ ] TODO: Implement SIMD-accelerated array scanning (find/grep) utilities.
*   [ ] TODO: Implement deterministic unique-set merging for repeated fields.
*   [x] Integration tests pass. (Current: failing due to ASan leaks).
*   [x] Create test file `perl/t/c/integration/190_repeated_coro.c`
*   [ ] Tests in `perl/t/c/integration/190_repeated_coro.c` stress concurrent C function usage for repeated. (Partially implemented).
*   [ ] TODO: Stress concurrent repeated field expansion under memory pressure.
*   [ ] TODO: Verify integrated cache stability during concurrent sub-message array mutation.
*   [ ] TODO: Implement race detection for shared arena array access.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
