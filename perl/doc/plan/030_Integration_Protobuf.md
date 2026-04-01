# Milestone 3: C Layer - Integration Tests (protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/030_protobuf.c`.
*   [ ] Tests cover interactions between obj_cache, arena, and utils. (Partially implemented).
*   [ ] TODO: Verify arena-sharing integrity across multiple messages.
*   [ ] TODO: Implement cross-interpreter isolation verification for core utilities.
*   [ ] TODO: Verify interrupt resilience (croak/longjmp safety) during upb operations.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/030_protobuf_coro.c` using libcoro.
*   [ ] Tests in `perl/t/c/integration/030_protobuf_coro.c` stress concurrent C function usage. (Partially implemented).
*   [ ] TODO: Verify lock-free progression for cache lookups under high contention.
*   [ ] TODO: Implement automated race detection for internal core state.
*   [x] libcoro integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
