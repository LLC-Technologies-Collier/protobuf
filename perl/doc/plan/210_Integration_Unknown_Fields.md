# Milestone 21: C Layer - Integration Tests (unknown_fields + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/210_unknown_fields.c`.
*   [ ] Tests cover unknown fields interactions with messages. (Partially implemented).
*   [ ] TODO: Implement selective Unknown Field Scrubbing by tag.
*   [ ] TODO: Verify integrated Unknown-to-Message reflection safety.
*   [ ] TODO: Verify unknown field preservation across deep-message merges.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/210_unknown_fields_coro.c`
*   [ ] Tests in `perl/t/c/integration/210_unknown_fields_coro.c` stress concurrent C function usage for unknown_fields. (Partially implemented).
*   [ ] TODO: Stress concurrent unknown field growth under memory pressure.
*   [ ] TODO: Verify integrated cache stability during concurrent unknown reification.
*   [ ] TODO: Implement race detection for concurrent unknown field analysis.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
