# Milestone 5: C Layer - Integration Tests (convert + protobuf)

[TOC]

*   [x] Create test file `t/c/integration/050_convert.c`.
*   [x] Tests cover interactions between convert functions and core utilities (arena, cache).
*   [x] Create test file `t/c/integration/050_convert_coro.c`
*   [x] Tests in `t/c/integration/050_convert_coro.c` use libcoro to stress concurrent C function usage for convert. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
