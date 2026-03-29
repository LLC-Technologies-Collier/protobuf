# Milestone 9: C Layer - Integration Tests (descriptor_containers + previous)

[TOC]

*   [x] Create test file `t/c/integration/090_descriptor_containers.c`.
*   [x] Tests cover interactions with descriptor types, pool, containers, and core.
*   [x] Create test file `t/c/integration/090_descriptor_containers_coro.c`
*   [x] Tests in `t/c/integration/090_descriptor_containers_coro.c` use libcoro to stress concurrent C function usage for descriptor_containers. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
