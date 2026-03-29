# Milestone 7: C Layer - Integration Tests (descriptor + previous)

[TOC]

*   [x] Create test file `t/c/integration/070_descriptor.c`.
*   [x] Tests cover interactions between descriptor types, convert, and core utilities.
*   [x] Create test file `t/c/integration/070_descriptor_coro.c`
*   [x] Tests in `t/c/integration/070_descriptor_coro.c` use libcoro to stress concurrent C function usage for descriptor. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
