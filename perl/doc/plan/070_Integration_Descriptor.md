# Milestone 7: C Layer - Integration Tests (descriptor + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/070_descriptor.c`.
*   [x] Tests cover interactions between descriptor types, convert, and core utilities. (Verified enum/message subdef resolution).
*   [x] Verify integrated Object Cache identity for all descriptor types.
*   [ ] TODO: Implement integrated EnumValueDef and OneofDef resolution tests. (Difficulty: 4/10).
*   [ ] TODO: Verify DescriptorPool lifetime safety during active descriptor usage. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/070_descriptor_coro.c`
*   [ ] Tests in `perl/t/c/integration/070_descriptor_coro.c` use libcoro to stress concurrent C function usage for descriptor. (Partially implemented).
*   [ ] TODO: Define test schema with complex circular cross-references. (Difficulty: 2/10).
*   [ ] TODO: Implement libcoro test to resolve these cross-references concurrently. (Difficulty: 3/10).
*   [ ] TODO: Verify resolution consistency and thread safety. (Difficulty: 2/10).
*   [ ] TODO: Create a high-concurrency test that repeatedly accesses the descriptor cache. (Difficulty: 3/10).
*   [ ] TODO: Check for race conditions or cache corruption during concurrent access. (Difficulty: 3/10).
*   [ ] TODO: Benchmark cache hit rate and latency under load. (Difficulty: 2/10).
*   [ ] TODO: Test logic to dispose of a DescriptorPool while descriptors are still being accessed. (Difficulty: 3/10).
*   [ ] TODO: Verify that reference counting or other safety mechanisms prevent crashes. (Difficulty: 3/10).
*   [ ] TODO: Implement stress test for rapid pool creation and destruction. (Difficulty: 2/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Research upb's support for reloading descriptors in an existing pool. (Difficulty: 3/10).
    *   [ ] TODO: Implement logic to update existing descriptors with new definitions. (Difficulty: 3/10).
    *   [ ] TODO: Ensure existing message objects remain compatible or are correctly invalidated. (Difficulty: 3/10).
    *   [ ] TODO: Test re-binding with both simple and complex schema changes. (Difficulty: 3/10).
    *   [ ] TODO: Design batch resolution API for multiple descriptors. (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized resolution logic to minimize cache misses. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark vectorized resolution against sequential resolution. (Difficulty: 2/10).
    *   [ ] TODO: Define integrity constraints for DescriptorPool and its descriptors. (Difficulty: 3/10).
    *   [ ] TODO: Implement background audit task to verify these constraints. (Difficulty: 3/10).
    *   [ ] TODO: Logic to detect and log (or fix) any integrity violations. (Difficulty: 3/10).
    *   [ ] TODO: Test the auditor under various failure scenarios. (Difficulty: 2/10).
    *   [ ] **Snapshotting:** Implement Read-Only DescriptorPool Snapshots. (Difficulty: 8/10)
    *   [ ] **Lazy Loading:** Implement Lazy Loading of FileDescriptorProtos. (Difficulty: 7/10)
    *   [ ] **Visualization:** Add function to dump DescriptorPool to Graphviz DOT. (Difficulty: 5/10)
