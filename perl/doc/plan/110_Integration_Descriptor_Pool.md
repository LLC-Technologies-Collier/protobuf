# Milestone 11: C Layer - Integration Tests (descriptor_pool + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/110_descriptor_pool.c`.
*   [x] Tests cover interactions between pool, descriptors, containers, and core. (Verified message/file identity and lookup).
*   [ ] TODO: Implement Thread-Safe Global Pool sharing across interpreters. (Difficulty: 6/10).
*   [ ] TODO: Implement Descriptor Set Lazy Loading for memory efficiency. (Difficulty: 7/10).
*   [ ] TODO: Implement detailed Cross-Pool Conflict Resolution reporting. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/110_descriptor_pool_coro.c`
*   [ ] Tests in `perl/t/c/integration/110_descriptor_pool_coro.c` use libcoro to stress concurrent C function usage for descriptor_pool. (Partially implemented).
*   [ ] TODO: Stress concurrent descriptor set addition with conflicting names. (Difficulty: 6/10).
*   [ ] TODO: Verify pool stability during high-frequency concurrent lookup of non-existent types. (Difficulty: 6/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Cross-Pool Definition Migration safety verification. (Difficulty: 8/10).
    *   [ ] TODO: Implement Schema Evolution Safety Guard for incompatible updates. (Difficulty: 7/10).
    *   [ ] TODO: Implement Global Schema Versioning for consistent object re-parsing. (Difficulty: 5/10).

