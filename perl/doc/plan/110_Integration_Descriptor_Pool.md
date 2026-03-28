# Milestone 11: C Layer - Integration Tests (descriptor_pool + previous)

[TOC]

*   [ ] Create test file `t/c/integration/110_descriptor_pool.c`.
*   [ ] Tests cover interactions between pool, descriptors, containers, and core.
*   [ ] Create test file `t/c/integration/110_descriptor_pool_coro.c`
*   [ ] Tests in `t/c/integration/110_descriptor_pool_coro.c` use libcoro to stress concurrent C function usage for descriptor_pool. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [ ] libcoro integration tests pass.
*   [ ] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
