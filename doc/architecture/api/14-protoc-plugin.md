# protoc Plugin Architecture

_Status: Planned_

The `protoc-gen-perl-pb` plugin is responsible for generating Perl modules from `.proto` files.

## Implementation

The plugin follows the standard `protoc` plugin protocol:
- Reads a `CodeGeneratorRequest` from STDIN.
- Processes the `FileDescriptorProto` objects.
- Writes a `CodeGeneratorResponse` to STDOUT.

### Core Logic
- **Module Generation**: Creates a `.pm` file for each Protobuf package or file.
- **Dynamic Initialization**: Generates code that loads the serialized descriptor set into the `Protobuf::DescriptorPool` and triggers the creation of Perl classes via `Protobuf::ClassGenerator`.
- **Moo Integration**: The generated modules utilize `Moo` for inheritance and attribute management.

## Advanced Generation Goals

To achieve world-class developer ergonomics and performance, the plugin includes (or is planned to include) the following:

- **C-Layer Code Generation**: (Planned) Implement the plugin using a high-performance C-layer generator that utilizes `upb` descriptors directly, ensuring 100% logic alignment with the runtime library.
- **Integrated Async Services**: (Planned) Automatically generate asynchronous service stubs for `Mojo::IOLoop` and `Coro` when services are defined in the `.proto` file.
- **Embedded Schema Metadata**: (Planned) Embed full serialized descriptor data within each generated module to allow for "offline" reflection and self-describing modules.
- **Type::Tiny Library Generation**: (Planned) Automatically generate corresponding `Type::Tiny` type libraries for each package to support rigorous business logic validation.

## Build Integration

- The plugin will be distributable as a standalone executable.
- Support for `Module::Build` and `ExtUtils::MakeMaker` integration to automate code generation during the Perl build process.
