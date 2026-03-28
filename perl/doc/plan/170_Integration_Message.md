# Milestone 17: C Layer - Integration Tests (message + previous)

[TOC]

*   [ ] Create test file `t/c/integration/170_message.c`.
*   [ ] Tests cover message interactions with all other components.
*   [ ] Create test file `t/c/integration/170_message_coro.c`
*   [ ] Tests in `t/c/integration/170_message_coro.c` use libcoro to stress concurrent C function usage for message. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [ ] libcoro integration tests pass.
*   [ ] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
