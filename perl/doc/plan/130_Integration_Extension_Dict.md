# Milestone 13: C Layer - Integration Tests (extension_dict + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/130_extension_dict.c`.
*   [x] Tests cover interactions with messages and extensions. (Verified initial get, set/get roundtrip, and iteration).
*   [ ] TODO: Implement Cross-Arena Extension Copy Stress tests. (Difficulty: 6/10).
*   [ ] TODO: Implement Audit/Trace for Extension Access. (Difficulty: 4/10).
*   [ ] TODO: Verify ExtensionDict stability during re-parsing of parent message. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/130_extension_dict_coro.c`
*   [ ] Tests in `perl/t/c/integration/130_extension_dict_coro.c` use libcoro to stress concurrent C function usage for extension_dict. (Partially implemented).
*   [ ] TODO: Stress concurrent extension mutation safety. (Difficulty: 6/10).
*   [ ] TODO: Verify integrated cache stability during concurrent extension access. (Difficulty: 6/10).
*   [ ] TODO: Stress concurrent extension dictionary iterator creation. (Difficulty: 5/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Predictive Extension Pre-fetching during parsing. (Difficulty: 7/10).
    *   [ ] TODO: Implement Self-Healing Extension Consistency background auditing. (Difficulty: 8/10).
    *   [ ] TODO: Implement Vectorized Bulk Extension Access for high-throughput projection. (Difficulty: 8/10).
