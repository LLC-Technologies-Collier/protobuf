# Milestone 7: C Layer - Integration Tests (descriptor + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/070_descriptor.c`.
*   [ ] Tests cover interactions between descriptor types, convert, and core utilities. (Partially implemented).
*   [ ] TODO: Verify integrated Object Cache identity for all descriptor types.
*   [ ] TODO: Implement integrated EnumValueDef and OneofDef resolution tests.
*   [ ] TODO: Verify DescriptorPool lifetime safety during active descriptor usage.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/070_descriptor_coro.c`
*   [ ] Tests in `perl/t/c/integration/070_descriptor_coro.c` use libcoro to stress concurrent C function usage for descriptor. (Partially implemented).
*   [ ] TODO: Stress concurrent cross-reference resolution between interdependent descriptors.
*   [ ] TODO: Verify descriptor cache stability under high concurrent load.
*   [ ] TODO: Implement concurrent DescriptorPool disposal safety verification.
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.
