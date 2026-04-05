# Milestone 42: `protoc` Plugin

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Design and implement `protoc-gen-perl-pb`.
    *   [x] **Core Logic (C/C++):**
        *   [x] Implement main plugin entry point to interact with `protoc` via stdin/stdout using `CodeGeneratorRequest` and `CodeGeneratorResponse`. (Placeholder in `perl/protoc/protoc-gen-perl-pb.c`)
        *   [ ] TODO: Use `upb_DefPool` to load and process the `FileDescriptorProto`s provided by `protoc`.
        *   [ ] TODO: Traverse the descriptors (Messages, Enums, Services, Fields, Extensions).
    *   [ ] **Template Engine:**
        *   [ ] TODO: Integrate a C/C++ templating engine (e.g., inja, ctemplate) or use simple string manipulation for code generation.
        *   [ ] TODO: Develop templates for `.pm` files.
    *   [ ] **Perl Module Generation (`.pm`):**
        *   [ ] TODO: Generate `package` declarations.
        *   [ ] TODO: Generate `use Moo;` and `extends 'Protobuf::Message';`.
        *   [ ] TODO: Generate `use Protobuf::Internal qw(:all);`.
        *   [ ] TODO: Embed serialized `FileDescriptorProto` data (e.g., base64 encoded) for runtime loading.
        *   [ ] TODO: Add code to load the embedded descriptor into the default `Protobuf::DescriptorPool` at module load time.
        *   [ ] TODO: Implement logic to register the generated class with the `Protobuf::ClassGenerator`.
    *   [ ] **Field Accessors:** (Handled by `Protobuf::Message` and `Protobuf::ClassGenerator` at runtime based on the loaded descriptor).
    *   [ ] **Enum Handling:**
        *   [ ] TODO: Generate constants for enum values within the message package or a separate Enum package.
    *   [ ] **Service Generation (Optional):**
        *   [ ] TODO: Add flags to enable/disable service generation.
        *   [ ] TODO: Template for basic service stubs (e.g., for gRPC or Twirp-like interfaces).
        *   [ ] Add Mojo-specific service templates to protoc-gen-perl. (Difficulty: 3/10).
        *   [ ] Implement Coro-specific service templates. (Difficulty: 3/10).
    *   [x] **Options & Configuration:**
        *   [ ] TODO: Handle standard protobuf options.
        *   [ ] TODO: Implement command-line flags for the plugin (e.g., output directory, service generation options).
        *   [x] Add flag to embed binary descriptors in generated PMs. (Difficulty: 2/10) - Option parsing added.
        *   [ ] TODO: Implement loader logic for embedded descriptors. (Difficulty: 3/10).
*   [x] Create test file `t/98-generated.t`. (Placeholder created)
*   [ ] Tests for generated code `t/98-generated.t`:
    *   [ ] Verify correct package and import statements.
    *   [ ] Test instantiation of generated classes.
    *   [ ] Verify field accessors (get/set/has/clear) work.
    *   [ ] Test serialization and deserialization of generated message types.
    *   [ ] Check enum constant values.
    *   [ ] Validate embedded descriptor loading.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add VPP-style dispatch templates to the generator. (Difficulty: 3/10).
    *   [ ] Implement vectorized field accessor templates. (Difficulty: 3/10).
    *   [ ] Implement static XS accessor generation for fixed fields. (Difficulty: 3/10).
    *   [ ] Add hot-path detection to the code generator. (Difficulty: 3/10).
    *   [ ] Implement shared memory service stubs. (Difficulty: 3/10).
    *   [ ] Add zero-copy IPC transport templates. (Difficulty: 3/10).
    *   [ ] Performance: Precompute hashes for field names in generated code. (Difficulty: 4/10).
    *   [ ] Observability: Embed source code info (proto file/line) in generated PMs. (Difficulty: 3/10).
    *   [ ] Safety: Generate Type::Tiny constraints for messages. (Difficulty: 6/10).
*   [ ] Review and update perl/doc/architecture/** documents.