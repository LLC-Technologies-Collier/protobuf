# Milestone 13: C Layer - Integration Tests (extension_dict + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/130_extension_dict.c`.
*   [x] Tests cover interactions with messages and extensions.
*   [x] Create test file `perl/t/c/integration/130_extension_dict_coro.c`
*   [x] Tests in `perl/t/c/integration/130_extension_dict_coro.c` use libcoro to stress concurrent C function usage for extension_dict. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
