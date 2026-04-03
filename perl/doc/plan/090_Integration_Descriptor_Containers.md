# Milestone 9: C Layer - Integration Tests (descriptor_containers + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/090_descriptor_containers.c`.
*   [x] Tests cover interactions with descriptor types, pool, containers, and core. (Verified ByNameMap, Sequence, Iterator).
*   [ ] TODO: Implement integrated ByNumberMap tests for EnumValue definitions. (Difficulty: 4/10).
*   [x] Verify integrated Iterator performance and stability for real definitions.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/090_descriptor_containers_coro.c`
*   [ ] Tests in `perl/t/c/integration/090_descriptor_containers_coro.c` use libcoro to stress concurrent C function usage for descriptor_containers. (Partially implemented).
*   [ ] TODO: Script to create and destroy thousands of descriptor containers in parallel (libcoro). (Difficulty: 3/10).
*   [ ] TODO: Monitor memory usage and check for leaks or fragmentation. (Difficulty: 2/10).
*   [ ] TODO: Verify container validity and content during high-concurrency churn. (Difficulty: 2/10).
*   [ ] TODO: Create a test that accesses shared descriptor containers from multiple coroutines. (Difficulty: 3/10).
*   [ ] TODO: Check for race conditions in the internal cache used by these containers. (Difficulty: 3/10).
*   [ ] TODO: Validate that cache hits return consistent data under load. (Difficulty: 2/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Design XS logic to map upb's internal sequence representation to a Perl array. (Difficulty: 3/10).
    *   [ ] TODO: Implement lazy-loading or proxying for large sequences to avoid full copy. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark direct projection against manual element-by-element copy. (Difficulty: 2/10).
    *   [ ] TODO: Create a test with multiple Perl interpreters using the same C containers. (Difficulty: 3/10).
    *   [ ] TODO: Verify that changes in one interpreter don't leak or affect another's state. (Difficulty: 3/10).
    *   [ ] TODO: Stress concurrent access to shared descriptors across interpreter boundaries. (Difficulty: 3/10).
    *   [ ] TODO: Research weak reference implementation in Perl XS for C-level objects. (Difficulty: 3/10).
    *   [ ] TODO: Implement logic to invalidate iterators when their parent container is destroyed. (Difficulty: 3/10).
    *   [ ] TODO: Logic to detect and handle "stale" iterator access gracefully without crashing. (Difficulty: 3/10).
    *   [ ] TODO: Test iterator self-healing with complex parent-child lifetime scenarios. (Difficulty: 3/10).
