# Milestone 11: C Layer - Integration Tests (descriptor_pool + previous)

[TOC]

*   [x] Create test file `t/c/integration/110_descriptor_pool.c`.
*   [x] Tests cover interactions between pool, descriptors, containers, and core.
*   [x] Create test file `t/c/integration/110_descriptor_pool_coro.c`
*   [x] Tests in `t/c/integration/110_descriptor_pool_coro.c` use libcoro to stress concurrent C function usage for descriptor_pool. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
