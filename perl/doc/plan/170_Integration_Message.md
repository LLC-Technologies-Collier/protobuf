# Milestone 17: C Layer - Integration Tests (message + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/170_message.c`.
*   [x] Tests cover message interactions with all other components.
*   [x] Create test file `perl/t/c/integration/170_message_coro.c`
*   [x] Tests in `perl/t/c/integration/170_message_coro.c` use libcoro to stress concurrent C function usage for message. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
