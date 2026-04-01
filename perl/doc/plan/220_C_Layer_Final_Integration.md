# Milestone 22: C Layer - Final Integration

[TOC]

*   [x] Create test file `perl/t/c/integration/220_all_c_layers.c`
*   [x] Tests exercise combinations of all C layer components. (Note: Extensions skipped for Serialize/Parse cycle in this milestone due to missing registry support).
*   [x] Create test file `perl/t/c/integration/220_all_c_layers_coro.c` using libcoro to stress all components. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro final integration tests pass.
*   [x] All C layer integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
