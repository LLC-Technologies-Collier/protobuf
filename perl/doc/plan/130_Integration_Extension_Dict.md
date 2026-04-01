# Milestone 13: C Layer - Integration Tests (extension_dict + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/130_extension_dict.c`.
*   [ ] Tests cover interactions with messages and extensions. (Partially implemented).
*   [ ] TODO: Implement Cross-Arena Extension Copy Stress tests.
*   [ ] TODO: Implement Audit/Trace for Extension Access.
*   [ ] TODO: Verify ExtensionDict stability during re-parsing of parent message.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/130_extension_dict_coro.c`
*   [ ] Tests in `perl/t/c/integration/130_extension_dict_coro.c` use libcoro to stress concurrent C function usage for extension_dict. (Partially implemented).
*   [ ] TODO: Stress concurrent extension mutation safety.
*   [ ] TODO: Verify integrated cache stability during concurrent extension access.
*   [ ] TODO: Stress concurrent extension dictionary iterator creation.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
