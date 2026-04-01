# Milestone 3: C Layer - Integration Tests (protobuf)

[TOC]

*   [x] Create test file `t/c/integration/030_protobuf.c`.
*   [ ] Tests cover interactions between obj_cache, arena, and utils. (Partially implemented).
*   [x] Integration tests pass.
*   [x] Create test file `t/c/integration/030_protobuf_coro.c` using libcoro.
*   [ ] Tests in `t/c/integration/030_protobuf_coro.c` stress concurrent C function usage for obj_cache, arena, and utils. (Note: Coro is used here for C-level stress testing to ensure re-entrancy and safety. The Perl API will aim to be event-loop agnostic.)
*   [x] libcoro integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.
