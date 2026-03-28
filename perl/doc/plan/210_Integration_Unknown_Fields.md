# Milestone 21: C Layer - Integration Tests (unknown_fields + previous)

[TOC]

*   [ ] Create test file `t/c/integration/210_unknown_fields.c`.
*   [ ] Tests cover unknown fields interactions with messages.
*   [ ] Create test file `t/c/integration/210_unknown_fields_coro.c`
*   [ ] Tests in `t/c/integration/210_unknown_fields_coro.c` use libcoro to stress concurrent C function usage for unknown_fields. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [ ] libcoro integration tests pass.
*   [ ] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
