# Milestone 5: C Layer - Integration Tests (convert + protobuf)

[TOC]

*   [x] Create test file `perl/t/c/integration/050_convert.c`.
*   [x] Tests cover interactions between convert functions and core utilities (arena, cache). (Verified: int32, string, uint32, bool).
*   [ ] TODO: Implement exhaustive primitive roundtrip tests for all 18 UPB types.
*   [x] TODO: Verify integrated Object Cache identity for message conversions.
*   [ ] TODO: Verify Math::BigInt roundtrip support in integrated context.
*   [x] Create test file `perl/t/c/integration/050_convert_coro.c`
*   [ ] Tests in `perl/t/c/integration/050_convert_coro.c` use libcoro to stress concurrent C function usage for convert. (Partially implemented).
*   [ ] TODO: Stress concurrent type-mismatch failures in SvToUpb.
*   [ ] TODO: Verify integrated cache stability under high concurrent load.
*   [ ] TODO: Implement concurrent memory pressure stress during string conversion.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
