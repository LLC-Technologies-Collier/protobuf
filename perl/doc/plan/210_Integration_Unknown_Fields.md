# Milestone 21: C Layer - Integration Tests (unknown_fields + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/210_unknown_fields.c`.
*   [x] Tests cover unknown fields interactions with messages. (Verified parsing, retrieval, reserialization, and manual addition).
*   [ ] TODO: Implement selective Unknown Field Scrubbing by tag. (Difficulty: 6/10).
*   [ ] TODO: Verify integrated Unknown-to-Message reflection safety. (Difficulty: 7/10).
*   [ ] TODO: Verify unknown field preservation across deep-message merges. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/210_unknown_fields_coro.c`
*   [ ] Tests in `perl/t/c/integration/210_unknown_fields_coro.c` use libcoro to stress concurrent C function usage for unknown_fields. (Partially implemented).
*   [ ] TODO: Stress concurrent unknown field growth under memory pressure. (Difficulty: 7/10).
*   [ ] TODO: Verify integrated cache stability during concurrent unknown reification. (Difficulty: 6/10).
*   [ ] TODO: Implement race detection for concurrent unknown field analysis. (Difficulty: 9/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Selective Unknown Field Scrubbing by tag list. (Difficulty: 7/10).
    *   [ ] TODO: Implement Unknown-to-Message Reflection Safety verification. (Difficulty: 8/10).
    *   [ ] TODO: Implement Vectorized Unknown Field Analysis for bulk tree scanning. (Difficulty: 8/10).

