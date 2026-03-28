# Milestone 5: C Layer - Integration Tests (convert + protobuf)

[TOC]

*   [ ] Create test file `t/c/integration/050_convert.c`.
*   [ ] Tests cover interactions between convert functions and core utilities (arena, cache).
*   [ ] Create test file `t/c/integration/050_convert_coro.c`
*   [ ] Tests in `t/c/integration/050_convert_coro.c` use libcoro to stress concurrent C function usage for convert. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [ ] libcoro integration tests pass.
*   [ ] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
