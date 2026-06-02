# Milestone 11: C Layer - Integration Tests (descriptor_pool + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/110_descriptor_pool.c`.
*   [x] Tests cover interactions between pool, descriptors, containers, and core. (Verified message/file identity and lookup).
*   [ ] TODO: Implement Thread-Safe Global Pool sharing across interpreters. (Difficulty: 6/10).
    *   [ ] TODO: Implement mutex-protected global descriptor pool instance. (Difficulty: 3/10).
    *   [ ] TODO: Implement reference counting for global pool across interpreters. (Difficulty: 3/10).
    *   [ ] TODO: Verify pool consistency when accessed from multiple threads/interpreters. (Difficulty: 3/10).
*   [ ] TODO: Implement Descriptor Set Lazy Loading for memory efficiency. (Difficulty: 7/10).
    *   [ ] TODO: Implement lazy loading mechanism for file descriptors. (Difficulty: 3/10).
    *   [ ] TODO: Verify on-demand descriptor loading during type lookup. (Difficulty: 3/10).
    *   [ ] TODO: Implement memory usage tracking and eviction policy for lazy-loaded sets. (Difficulty: 3/10).
*   [ ] TODO: Implement detailed Cross-Pool Conflict Resolution reporting. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/110_descriptor_pool_coro.c`
*   [ ] Tests in `perl/t/c/integration/110_descriptor_pool_coro.c` use libcoro to stress concurrent C function usage for descriptor_pool. (Partially implemented).
*   [ ] TODO: Stress concurrent descriptor set addition with conflicting names. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent descriptor addition test using libcoro. (Difficulty: 3/10).
    *   [ ] TODO: Verify error handling and recovery during name conflicts under concurrency. (Difficulty: 3/10).
    *   [ ] TODO: Verify pool integrity after failed concurrent additions. (Difficulty: 3/10).
*   [ ] TODO: Verify pool stability during high-frequency concurrent lookup of non-existent types. (Difficulty: 6/10).
    *   [ ] TODO: Implement stress test for concurrent non-existent type lookups. (Difficulty: 3/10).
    *   [ ] TODO: Verify no memory leaks during negative lookup caching. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark lookup performance under heavy contention. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Cross-Pool Definition Migration safety verification. (Difficulty: 8/10).
        *   [ ] TODO: Research requirements and constraints for cross-pool migration. (Difficulty: 3/10).
        *   [ ] TODO: Implement validation logic to ensure definition compatibility during migration. (Difficulty: 3/10).
        *   [ ] TODO: Verify that migrated definitions correctly reference new pool dependencies. (Difficulty: 3/10).
    *   [ ] TODO: Implement Schema Evolution Safety Guard for incompatible updates. (Difficulty: 7/10).
        *   [ ] TODO: Implement detection of incompatible schema changes (e.g., field type changes). (Difficulty: 3/10).
        *   [ ] TODO: Implement warning/error reporting for detected incompatibilities. (Difficulty: 3/10).
        *   [ ] TODO: Implement optional strict mode for enforcing schema evolution rules. (Difficulty: 3/10).
    *   [ ] TODO: Implement Global Schema Versioning for consistent object re-parsing. (Difficulty: 5/10).

