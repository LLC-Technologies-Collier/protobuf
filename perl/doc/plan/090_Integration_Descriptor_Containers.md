# Milestone 9: C Layer - Integration Tests (descriptor_containers + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/090_descriptor_containers.c`.
*   [x] Tests cover interactions with descriptor types, pool, containers, and core. (Verified ByNameMap, Sequence, Iterator).
*   [ ] TODO: Implement integrated ByNumberMap tests for EnumValue definitions. (Difficulty: 4/10).
*   [x] Verify integrated Iterator performance and stability for real definitions.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/090_descriptor_containers_coro.c`
*   [ ] Tests in `perl/t/c/integration/090_descriptor_containers_coro.c` use libcoro to stress concurrent C function usage for descriptor_containers. (Partially implemented).
*   [ ] TODO: Implement concurrent large-scale temporary container creation stress. (Difficulty: 6/10).
*   [ ] TODO: Verify integrated cache integrity during concurrent descriptor container access. (Difficulty: 6/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Direct C-to-Perl Projection for Descriptor Sequence (GenericSequence_AsArray). (Difficulty: 7/10).
    *   [ ] TODO: Implement Cross-Interpreter Container Isolation Stress verification. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing Iterator State using weak-ref monitoring for stale parents. (Difficulty: 9/10).
