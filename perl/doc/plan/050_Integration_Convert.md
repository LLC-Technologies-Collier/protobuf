# Milestone 5: C Layer - Integration Tests (convert + protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/050_convert.c`.
*   [x] Tests cover interactions between convert functions and core utilities (arena, cache). (Verified: int32, string, uint32, bool).
*   [ ] TODO: Implement exhaustive primitive roundtrip tests for all 18 UPB types. (Difficulty: 4/10).
*   [x] Verify integrated Object Cache identity for message conversions.
*   [ ] TODO: Verify Math::BigInt roundtrip support in integrated context. (Difficulty: 3/10).
*   [x] Create test file `perl/t/c/integration/050_convert_coro.c`
*   [ ] Tests in `perl/t/c/integration/050_convert_coro.c` use libcoro to stress concurrent C function usage for convert. (Partially implemented).
*   [ ] TODO: Stress concurrent type-mismatch failures in SvToUpb. (Difficulty: 5/10).
*   [ ] TODO: Verify integrated cache stability under high concurrent load. (Difficulty: 6/10).
*   [ ] TODO: Implement concurrent memory pressure stress during string conversion. (Difficulty: 7/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Cross-Interpreter Object Migration safety verification. (Difficulty: 8/10).
    *   [ ] TODO: Implement Predictive Field Pre-fetching to minimize accessor latency. (Difficulty: 7/10).
    *   [ ] TODO: Implement SIMD-accelerated Batch Conversion (VPP style). (Difficulty: 9/10).
