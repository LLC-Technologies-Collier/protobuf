# Milestone 15: C Layer - Integration Tests (map + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/150_map.c`.
*   [x] Tests cover map interactions with messages. (Verified int32, string, and AsHash projection).
*   [ ] TODO: Implement Sub-message map value roundtrip with ObjCache identity. (Difficulty: 5/10).
*   [x] Implement O(1) bulk projection of upb_Map to Perl HV.
*   [ ] TODO: Verify integrated map stability during concurrent field deletion:
    *   [ ] TODO: Implement `libcoro` task for randomized map key deletion. (Difficulty: 2/10)
    *   [ ] TODO: Verify map size consistency across concurrent deletion workers. (Difficulty: 2/10)
    *   [ ] TODO: Integrate ASan to detect use-after-free during deletion. (Difficulty: 2/10)
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/150_map_coro.c`
*   [ ] Tests in `perl/t/c/integration/150_map_coro.c` use libcoro to stress concurrent C function usage for map. (Partially implemented).
*   [ ] TODO: Stress concurrent map mutation and integrated iterator stability:
    *   [ ] TODO: Implement concurrent iterator traversal during active map mutation. (Difficulty: 3/10)
    *   [ ] TODO: Verify iterator "snapshot" consistency or safe invalidation. (Difficulty: 2/10)
    *   [ ] TODO: Benchmark iterator performance under high mutation pressure. (Difficulty: 2/10)
*   [ ] TODO: Verify integrated cache integrity for map values under concurrency:
    *   [ ] TODO: Implement `ObjCache` stress test for concurrent map value retrieval. (Difficulty: 3/10)
    *   [ ] TODO: Verify singleton identity for map sub-message values across threads. (Difficulty: 2/10)
*   [ ] TODO: Implement concurrent memory pressure stress during map population:
    *   [ ] TODO: Create `libcoro` task for bulk map population with limited arena memory. (Difficulty: 3/10)
    *   [ ] TODO: Verify graceful OOM handling/recovery in C-layer map logic. (Difficulty: 2/10)
    *   [ ] TODO: Monitor arena block fragmentation during high-pressure map growth. (Difficulty: 2/10)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Lazy Map Entry Inflation for deferred wrapper creation:
        *   [ ] TODO: Implement C-level proxy for un-inflated map entries. (Difficulty: 3/10)
        *   [ ] TODO: Create XS logic for on-demand inflation during field access. (Difficulty: 3/10)
        *   [ ] TODO: Benchmark lazy vs. eager map entry population. (Difficulty: 2/10)
    *   [ ] TODO: Implement Cross-Arena Map Copy Stress verification:
        *   [ ] TODO: Implement `upb_Map_Copy` stress test between distinct arenas. (Difficulty: 3/10)
        *   [ ] TODO: Verify deep-copy integrity for sub-message map values. (Difficulty: 3/10)
        *   [ ] TODO: Integrate memory leak detection during bulk cross-arena copies. (Difficulty: 2/10)
    *   [ ] TODO: Implement SIMD-Accelerated Map Key Validation (SSE4.2):
        *   [ ] TODO: Implement SSE4.2 `pcmpestri` kernel for bulk string key validation. (Difficulty: 3/10)
        *   [ ] TODO: Create dispatcher for non-SSE fallback path. (Difficulty: 2/10)
        *   [ ] TODO: Benchmark SIMD-accelerated map ingestion vs. standard path. (Difficulty: 3/10)
