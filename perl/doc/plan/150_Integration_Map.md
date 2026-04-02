# Milestone 15: C Layer - Integration Tests (map + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/150_map.c`.
*   [x] Tests cover map interactions with messages. (Verified int32, string, and AsHash projection).
*   [ ] TODO: Implement Sub-message map value roundtrip with ObjCache identity.
*   [x] TODO: Implement O(1) bulk projection of upb_Map to Perl HV.
*   [ ] TODO: Verify integrated map stability during concurrent field deletion.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/150_map_coro.c`
*   [ ] Tests in `perl/t/c/integration/150_map_coro.c` use libcoro to stress concurrent C function usage for map. (Partially implemented).
*   [ ] TODO: Stress concurrent map mutation and integrated iterator stability.
*   [ ] TODO: Verify integrated cache integrity for map values under concurrency.
*   [ ] TODO: Implement concurrent memory pressure stress during map population.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
