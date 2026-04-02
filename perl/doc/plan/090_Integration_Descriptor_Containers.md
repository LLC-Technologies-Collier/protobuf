# Milestone 9: C Layer - Integration Tests (descriptor_containers + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/090_descriptor_containers.c`.
*   [x] Tests cover interactions with descriptor types, pool, containers, and core. (Verified ByNameMap, Sequence, Iterator).
*   [ ] TODO: Implement integrated ByNumberMap tests for EnumValue definitions.
*   [x] TODO: Verify integrated Iterator performance and stability for real definitions.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/090_descriptor_containers_coro.c`
*   [ ] Tests in `perl/t/c/integration/090_descriptor_containers_coro.c` use libcoro to stress concurrent C function usage for descriptor_containers. (Partially implemented).
*   [ ] TODO: Implement concurrent large-scale temporary container creation stress.
*   [ ] TODO: Verify integrated cache integrity during concurrent descriptor container access.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
