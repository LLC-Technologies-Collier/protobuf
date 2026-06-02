# Milestone 13: C Layer - Integration Tests (extension_dict + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/130_extension_dict.c`.
*   [x] Tests cover interactions with messages and extensions. (Verified initial get, set/get roundtrip, and iteration).
*   [ ] TODO: Implement Cross-Arena Extension Copy Stress tests. (Difficulty: 6/10).
    *   [ ] TODO: Implement basic cross-arena extension copy test case. (Difficulty: 3/10).
    *   [ ] TODO: Implement stress test for large volume of cross-arena extension copies. (Difficulty: 3/10).
    *   [ ] TODO: Verify memory integrity after high-frequency cross-arena extension copying. (Difficulty: 3/10).
*   [ ] TODO: Implement Audit/Trace for Extension Access. (Difficulty: 4/10).
*   [ ] TODO: Verify ExtensionDict stability during re-parsing of parent message. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/130_extension_dict_coro.c`
*   [ ] Tests in `perl/t/c/integration/130_extension_dict_coro.c` use libcoro to stress concurrent C function usage for extension_dict. (Partially implemented).
*   [ ] TODO: Stress concurrent extension mutation safety. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent extension mutation test using libcoro. (Difficulty: 3/10).
    *   [ ] TODO: Verify atomicity of extension updates under high concurrency. (Difficulty: 3/10).
    *   [ ] TODO: Implement race condition detection for extension mutation. (Difficulty: 3/10).
*   [ ] TODO: Verify integrated cache stability during concurrent extension access. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent read/write test for extension cache. (Difficulty: 3/10).
    *   [ ] TODO: Verify cache consistency after concurrent extension access. (Difficulty: 3/10).
    *   [ ] TODO: Implement cache invalidation stress test under concurrency. (Difficulty: 3/10).
*   [ ] TODO: Stress concurrent extension dictionary iterator creation. (Difficulty: 5/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Predictive Extension Pre-fetching during parsing. (Difficulty: 7/10).
        *   [ ] TODO: Research and design pre-fetching heuristics for extensions. (Difficulty: 3/10).
        *   [ ] TODO: Implement basic pre-fetching logic in the extension parser. (Difficulty: 3/10).
        *   [ ] TODO: Benchmark and tune pre-fetching performance for common access patterns. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Extension Consistency background auditing. (Difficulty: 8/10).
        *   [ ] TODO: Implement background consistency auditor for extensions. (Difficulty: 3/10).
        *   [ ] TODO: Implement self-healing logic for detected extension inconsistencies. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor's impact on performance and memory usage. (Difficulty: 3/10).
    *   [ ] TODO: Implement Vectorized Bulk Extension Access for high-throughput projection. (Difficulty: 8/10).
        *   [ ] TODO: Design API for bulk extension access. (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized read operations for multiple extensions. (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized write operations for multiple extensions. (Difficulty: 3/10).
