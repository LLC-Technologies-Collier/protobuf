# Milestone 42: `protoc` Plugin

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Design and implement `protoc-gen-perl-pb`.
    *   [x] **Core Logic (C++):**
        *   [x] Implement main plugin entry point to interact with `protoc` via stdin/stdout using `CodeGeneratorRequest` and `CodeGeneratorResponse`.
        *   [x] Use UPB to process descriptors and generate Perl code.
    *   [x] **Perl Module Generation (`.pm`):**
        *   [x] **Package Name:** Derive the Perl package name from the proto package.
        *   [x] **Embed Descriptor:** (Default: true)
            *   Serialize the `FileDescriptorProto` for the current file.
            *   Base64 encode the serialized string.
            *   Generate a `BEGIN` block to load the descriptor into the generated pool.
        *   [x] Trigger `Protobuf::ClassGenerator` at module load time via `add_serialized_file`.
*   [x] Create test file `t/98-generated.t`.
*   [x] Tests for generated code `t/98-generated.t`:
    *   [x] Verify correct package and import statements.
    *   [x] Test instantiation of generated classes.
    *   [x] Verify field accessors (get/set/has/clear) work.
    *   [x] Test serialization and deserialization of generated message types.
    *   [x] Check enum constant values.
    *   [x] Validate embedded descriptor loading.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add VPP-style dispatch templates to the generator. (Difficulty: 3/10).
    *   [ ] Implement vectorized field accessor templates. (Difficulty: 3/10).
    *   [ ] Implement static XS accessor generation for fixed fields. (Difficulty: 3/10).
    *   [ ] Add hot-path detection to the code generator. (Difficulty: 3/10).
    *   [ ] Implement shared memory service stubs. (Difficulty: 3/10).
    *   [ ] Add zero-copy IPC transport templates. (Difficulty: 3/10).
    *   [ ] Performance: Precompute hashes for field names in generated code. (Difficulty: 4/10).
    *   [ ] Observability: Embed source code info (proto file/line) in generated PMs. (Difficulty: 3/10).
    *   [ ] Safety: Generate Type::Tiny constraints for messages:
        *   [ ] Design Type::Tiny constraint generation strategy. (Difficulty: 3/10)
        *   [ ] Implement basic constraint generation for scalar types. (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Enum types. (Difficulty: 2/10)
        *   [ ] Implement constraint generation for Message types (e.g., `InstanceOf`). (Difficulty: 2/10)
        *   [ ] Implement constraint generation for Repeated fields (e.g., `ArrayRef`). (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Map fields (e.g., `HashRef`). (Difficulty: 3/10)
        *   [ ] Add tests for generated Type::Tiny constraints in `t/98-generated.t`. (Difficulty: 3/10)
*   [ ] Review and update perl/doc/architecture/** documents.