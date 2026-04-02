# Milestone 7: C Layer - Integration Tests (descriptor + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/070_descriptor.c`.
*   [x] Tests cover interactions between descriptor types, convert, and core utilities. (Verified enum/message subdef resolution).
*   [x] Verify integrated Object Cache identity for all descriptor types.
*   [ ] TODO: Implement integrated EnumValueDef and OneofDef resolution tests. (Difficulty: 4/10).
*   [ ] TODO: Verify DescriptorPool lifetime safety during active descriptor usage. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/070_descriptor_coro.c`
*   [ ] Tests in `perl/t/c/integration/070_descriptor_coro.c` use libcoro to stress concurrent C function usage for descriptor. (Partially implemented).
*   [ ] TODO: Stress concurrent cross-reference resolution between interdependent descriptors. (Difficulty: 6/10).
*   [ ] TODO: Verify descriptor cache stability under high concurrent load. (Difficulty: 6/10).
*   [ ] TODO: Implement concurrent DescriptorPool disposal safety verification. (Difficulty: 7/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Dynamic Symbol Re-binding for DescriptorPool reloading. (Difficulty: 8/10).
    *   [ ] TODO: Implement Vectorized Cross-Reference Resolution (VPP style). (Difficulty: 7/10).
    *   [ ] TODO: Implement Self-Healing Pool Integrity background auditing. (Difficulty: 9/10).
