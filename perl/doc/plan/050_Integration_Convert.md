# Milestone 5: C Layer - Integration Tests (convert + protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/050_convert.c`.
*   [x] Tests cover interactions between convert functions and core utilities (arena, cache). (Verified: int32, string, uint32, bool).
*   [ ] TODO: Implement exhaustive primitive roundtrip tests for all 18 UPB types. (Difficulty: 4/10).
*   [x] Verify integrated Object Cache identity for message conversions.
*   [ ] TODO: Verify Math::BigInt roundtrip support in integrated context. (Difficulty: 3/10).
*   [x] Create test file `perl/t/c/integration/050_convert_coro.c`
*   [ ] Tests in `perl/t/c/integration/050_convert_coro.c` use libcoro to stress concurrent C function usage for convert. (Partially implemented).
*   [ ] Add libcoro stress for SvToUpb type conversion. (Difficulty: 3/10).
*   [ ] Verify conversion stability under high context-switch rates. (Difficulty: 3/10).
*   [ ] Verify cache HIT/MISS ratio during concurrent conversion. (Difficulty: 3/10).
*   [ ] Implement thread-local locks for shared conversion buffers. (Difficulty: 3/10).
*   [ ] Add libcoro stress for string conversion with tight heap. (Difficulty: 3/10).
*   [ ] Implement fallback to slow-path during memory pressure. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add shared memory metadata for cross-interpreter objects. (Difficulty: 3/10).
    *   [ ] Implement `migrate_object()` utility in XS. (Difficulty: 3/10).
    *   [ ] Implement speculative field inflation based on schema hot-spots. (Difficulty: 3/10).
    *   [ ] Add metadata for accessor hot-path optimization. (Difficulty: 3/10).
    *   [ ] Implement VPP-style dispatch for bulk scalar conversion. (Difficulty: 3/10).
    *   [ ] Add AVX2 kernels for specific primitive type vectors. (Difficulty: 3/10).
