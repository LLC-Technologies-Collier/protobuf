# Milestone 42: `protoc` Plugin

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Design and implement `protoc-gen-perl-pb`.
*   [ ] Implement template-based code generation logic in C. (Difficulty: 3/10).
*   [ ] Add upb descriptor traversal for generator output. (Difficulty: 3/10).
*   [ ] Add Mojo-specific service templates to protoc-gen-perl. (Difficulty: 3/10).
*   [ ] Implement Coro-specific service templates. (Difficulty: 3/10).
*   [ ] Add flag to embed binary descriptors in generated PMs. (Difficulty: 2/10).
*   [ ] Implement loader logic for embedded descriptors. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add VPP-style dispatch templates to the generator. (Difficulty: 3/10).
    *   [ ] Implement vectorized field accessor templates. (Difficulty: 3/10).
    *   [ ] Implement static XS accessor generation for fixed fields. (Difficulty: 3/10).
    *   [ ] Add hot-path detection to the code generator. (Difficulty: 3/10).
    *   [ ] Implement shared memory service stubs. (Difficulty: 3/10).
    *   [ ] Add zero-copy IPC transport templates. (Difficulty: 3/10).
*   [x] Create test file `t/98-generated.t` (TODO stub created).
*   [ ] Tests for generated code (keywords, imports, etc.) `t/98-generated.t`.
*   [ ] Review and update perl/doc/architecture/** documents.