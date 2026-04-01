# Milestone 15: C Layer - Integration Tests (map + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/150_map.c`.
*   [x] Tests cover map interactions with messages.
*   [x] Create test file `perl/t/c/integration/150_map_coro.c`
*   [x] Tests in `perl/t/c/integration/150_map_coro.c` use libcoro to stress concurrent C function usage for map. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
