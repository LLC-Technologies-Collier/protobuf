# Milestone 42: `protoc` Plugin

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Design and implement `protoc-gen-perl-pb`.
    *   [ ] **Core Logic (C/C++):**
        *   [ ] Implement main plugin entry point to interact with `protoc` via stdin/stdout using `CodeGeneratorRequest` and `CodeGeneratorResponse`. (Placeholder in `perl/protoc/protoc-gen-perl-pb.c`)
        *   [ ] Use `upb_DefPool` to load and process the `FileDescriptorProto`s provided by `protoc`. (Pool creation/free added)
        *   [ ] Traverse the descriptors (Messages, Enums, Services, Fields, Extensions).
    *   [ ] **Template Engine:**
        *   [ ] TODO: Integrate a C/C++ templating engine (e.g., inja, ctemplate) or use simple string manipulation for code generation.
        *   [ ] TODO: Develop templates for `.pm` files.
    *   [ ] **Perl Module Generation (`.pm`):**
        *   [ ] **Module Naming & Path:**
            *   Implement `proto_path_to_module_path(proto_file, package)`: Converts `path/to/my_proto.proto` and package `my.package` to `My/Package/MyProto.pm`. Involves:
                *   Replacing `.` with `/` in the package for the directory structure.
                *   Extracting the base filename.
                *   Converting the base filename from `snake_case` to `CamelCase`.
            *   Implement `get_module_base_name(package, proto_file)`: Extracts the `My::Package` part from the package string.
            *   Helper functions `capitalize` and `to_camel_case` are needed.
        *   [ ] **Package Name:** Derive the Perl package name (e.g., `My::Package::MyProto`) from the module path.
        *   [ ] **Generate Header:** Output standard `package` line, `use strict;`, `use warnings;`, `use Protobuf::Message;`, `use Protobuf::Internal qw(:all);`.
        *   [ ] **Conditional Uses:** Add `use MIME::Base64;` if `embed_descriptors` is true. Add `use Const::Fast;` if there are enums.
        *   [ ] **Embed Descriptor:** (If `embed_descriptors` is true)
            *   Serialize the `FileDescriptorProto` for the current file.
            *   Base64 encode the serialized string (add `base64_encode` C++ function).
            *   Generate a `BEGIN` block in the Perl module.
            *   Embed the base64 string as a multi-line string assigned to `$descriptor_b64`.
            *   Add code to call `Protobuf::DescriptorPool::get_generated_pool()->add_serialized_file()` with the base64 decoded data.
        *   [ ] Add code to load the embedded descriptor into the default `Protobuf::DescriptorPool` at module load time.
        *   [ ] TODO: Implement logic to register the generated class with the `Protobuf::ClassGenerator`.
    *   [ ] **Field Accessors:** (Handled by `Protobuf::Message` and `Protobuf::ClassGenerator` at runtime based on the loaded descriptor).
    *   [ ] **Enum Handling:**
        *   [ ] Iterate through `enum_type` in the `FileDescriptorProto`.
        *   [ ] For each `EnumValueDescriptorProto`, generate a constant: `const my $VALUE_NAME => $value_number;`.
    *   [ ] **Service Generation (Optional):**
        *   [ ] Add flags to enable/disable service generation. (Option parsing added)
        *   [ ] Add placeholder comments in the generated code if `generate_services` is true and services are present.
        *   [ ] TODO: Template for basic service stubs (e.g., for gRPC or Twirp-like interfaces).
        *   [ ] Add Mojo-specific service templates to protoc-gen-perl. (Difficulty: 3/10).
        *   [ ] Implement Coro-specific service templates. (Difficulty: 3/10).
    *   [ ] **Options & Configuration:**
        *   [ ] Handle standard protobuf options.
        *   [ ] Implement command-line flags for the plugin (e.g., output directory, service generation options). (out_dir added)
        *   [ ] Add flag to embed binary descriptors in generated PMs. (Difficulty: 2/10) - Option parsing added.
        *   [ ] Implement loader logic for embedded descriptors. (Difficulty: 3/10).
*   [ ] Create test file `t/98-generated.t`. (Placeholder created)
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
    *   [ ] Safety: Generate Type::Tiny constraints for messages:
        *   [ ] Design Type::Tiny constraint generation strategy. (Difficulty: 3/10)
        *   [ ] Implement basic constraint generation for scalar types. (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Enum types. (Difficulty: 2/10)
        *   [ ] Implement constraint generation for Message types (e.g., `InstanceOf`). (Difficulty: 2/10)
        *   [ ] Implement constraint generation for Repeated fields (e.g., `ArrayRef`). (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Map fields (e.g., `HashRef`). (Difficulty: 3/10)
        *   [ ] Add tests for generated Type::Tiny constraints in `t/98-generated.t`. (Difficulty: 3/10)
*   [ ] Review and update perl/doc/architecture/** documents.