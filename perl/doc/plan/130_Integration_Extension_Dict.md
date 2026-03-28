# Milestone 13: C Layer - Integration Tests (extension_dict + previous)

[TOC]

*   [ ] Create test file `t/c/integration/130_extension_dict.c`.
*   [ ] Tests cover interactions with messages and extensions.
*   [ ] Create test file `t/c/integration/130_extension_dict_coro.c`
*   [ ] Tests in `t/c/integration/130_extension_dict_coro.c` use libcoro to stress concurrent C function usage for extension_dict. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [ ] libcoro integration tests pass.
*   [ ] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
