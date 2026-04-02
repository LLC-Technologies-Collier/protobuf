# Milestone 3: C Layer - Integration Tests (protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/030_protobuf.c`.
*   [x] Tests cover interactions between obj_cache, arena, and utils.
*   [x] Verify basic arena-sharing integrity.
*   [ ] TODO: Implement cross-interpreter isolation verification for core utilities. (Difficulty: 5/10).
*   [ ] TODO: Verify interrupt resilience (croak/longjmp safety) during upb operations. (Difficulty: 7/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/030_protobuf_coro.c` using libcoro.
*   [x] Tests in `perl/t/c/integration/030_protobuf_coro.c` stress concurrent C function usage.
*   [ ] TODO: Verify lock-free progression for cache lookups under high contention. (Difficulty: 6/10).
*   [ ] TODO: Implement automated race detection for internal core state. (Difficulty: 9/10).
*   [x] libcoro integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Distributed Shared Cache for cross-process object identity (O(1) IPC). (Difficulty: 8/10).
    *   [ ] TODO: Implement Predictive JIT Arena Warming to minimize L1 data cache misses. (Difficulty: 7/10).
    *   [ ] TODO: Implement Self-Healing Corruption Resilience for automated canary-based recovery. (Difficulty: 9/10).
