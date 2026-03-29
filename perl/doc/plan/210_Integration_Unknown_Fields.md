# Milestone 21: C Layer - Integration Tests (unknown_fields + previous)

[TOC]

*   [x] Create test file `t/c/integration/210_unknown_fields.c`.
*   [x] Tests cover unknown fields interactions with messages.
*   [x] Create test file `t/c/integration/210_unknown_fields_coro.c`
*   [x] Tests in `t/c/integration/210_unknown_fields_coro.c` use libcoro to stress concurrent C function usage for unknown_fields. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
