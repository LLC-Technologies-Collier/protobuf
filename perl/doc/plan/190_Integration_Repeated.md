# Milestone 19: C Layer - Integration Tests (repeated + previous)

[TOC]

*   [x] Create test file `t/c/integration/190_repeated.c`.
*   [x] Tests cover repeated field interactions with messages.
*   [x] Create test file `t/c/integration/190_repeated_coro.c`
*   [x] Tests in `t/c/integration/190_repeated_coro.c` use libcoro to stress concurrent C function usage for repeated. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
