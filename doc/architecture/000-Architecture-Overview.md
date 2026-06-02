# Perl Protobuf Architecture Overview

[TOC]

## 1. Overall Philosophy

This project implements a high-performance Perl extension for Protocol Buffers, leveraging the core `upb` C library. Key principles include:

*   **UPB-centric:** The `upb` library is the foundation for all C-level operations.
*   **Performance:** Designed for speed, using XS to bridge Perl and C, minimizing Perl-level overhead.
*   **Modern Perl:** Utilizes Moo for the object system in the Perl layer.
*   **Dynamic Class Generation:** Perl classes representing message types are generated dynamically at runtime when descriptors are loaded.
*   **Memory Safety:** Emphasizes arena-based memory management from `upb` and careful lifecycle management between Perl objects and C structures.
*   **Comprehensive Testing:** Includes both Perl-level unit/integration tests and extensive C-level tests.
*   **Python Parity:** Aims for feature and behavioral parity with the official Python UPB-based implementation.

## 2. Directory Structure (`perl/`)

*   **`lib/`**: Contains the Perl modules (`.pm` files) forming the public API (e.g., `Protobuf.pm`, `Protobuf/Message.pm`, `Protobuf/DescriptorPool.pm`, etc.) and their associated `.xs` files for XS bindings.
*   **`xs/`**: Contains pure C helper functions and implementations for various components (e.g., `convert`, `descriptor`, `message`, `protobuf` core utilities like arena and object cache). These are compiled into `libprotobufperl.so`.
*   **`t/`**: Perl-level test files (`.t`).
    *   **`t/c/`**: C-level test files (`.c`). These are compiled into standalone executables.
    *   **`t/integration/`**: Perl tests focusing on the interaction of multiple components.
    *   **`t/lib/`**: Helper modules for Perl tests.
*   **`xt/`**: Extended tests, including author tests, release tests, and concurrency tests for Coro, Mojo, etc.
*   **`inc/`**: Modules and templates used by the build system (`Makefile.PL`, `BUILD.bazel.PL`).
*   **`local/`**: Vendored Perl dependencies for the build system.
*   **`typemap`**: Contains the global typemap file for XS.
*   **`doc/`**: Project documentation, including plans and architecture.
*   **`protoc/`**: Source for the `protoc-gen-perl-pb` plugin.
*   **`libcoro/`**: Vendored source for the `libcoro` C coroutine library, used in C-level concurrency tests.

## 3. Build Systems

*   **`Makefile.PL`**: The primary build system using `ExtUtils::MakeMaker`. It compiles `../upb` sources, `../src` sources, and `perl/xs` sources into a central `libprotobufperl.so`. It also manages XS compilation and C test compilation. Template Toolkit is used within `MY::` overrides to generate complex Makefile rules from `.mk.tt` files in `inc/templates/`.
*   **`BUILD.bazel.PL`**: A script to generate `perl/BUILD.bazel`, integrating the Perl components with the main project's Bazel build. This is currently under development and aims to build `libprotobufperl`, C tests, and benchmarks.

## 4. Core Components

*   **`libprotobufperl.so`**: A shared C library built by `Makefile.PL`, containing the compiled UPB sources, common C utilities from `protobuf/src`, and custom C code from `perl/xs/`. This is linked by all XS modules.
*   **`Protobuf::*` Modules (`lib/`)**: The user-facing Perl classes.
*   **XS Bindings (`lib/Protobuf/*.xs`, `xs/`)**: The bridge between Perl and the C functions in `libprotobufperl.so`.
*   **Object Cache (`xs/protobuf/obj_cache.c`)**: Ensures that the same C pointer maps to the same Perl object, crucial for object identity. Uses a striped mutex design for concurrency.
*   **Arena Management (`xs/protobuf/arena.c`)**: Wraps `upb_Arena` for memory management.
*   **Type Conversion (`xs/convert/`)**: Handles marshalling between Perl SVs and UPB C types.

## 5. Testing Strategy

*   **C-Level Tests (`t/c/`)**: Standalone C programs testing the C API of `libprotobufperl` and interactions with UPB. Uses a custom harness (`t/c/upb-perl-test.h`).
*   **Perl Unit Tests (`t/`)**: Testing the Perl API of `Protobuf::*` modules.
*   **Integration Tests (`t/integration/`)**: Testing how different Perl modules interact.
*   **Concurrency Tests (`xt/coro/`, `xt/mojo/`)**: Testing thread and event loop safety.
*   **Sanitizers**: Encourages use of ASan and TSan during development.

## 6. Key Architectural Decisions & Features

*   **Dynamic Class Generation**: `Protobuf::ClassGenerator` creates Perl classes for messages on the fly when descriptors are loaded.
*   **Tied Interfaces**: `Protobuf::Internal::Repeated` and `Protobuf::Internal::Map` use `Tie::Array` and `Tie::Hash` to provide idiomatic Perl interfaces to the underlying `upb_Array` and `upb_Map`.
*   **Per-Interpreter Registry**: Centralized C struct to hold per-interpreter state like the object cache, minimizing global lookups.
*   **Advanced Memory Management**: Features like tmpfs-backed arenas for IPC, memory canaries, and block allocators are implemented.
*   **Engineering Excellence Goals**: The project has numerous "Engineering Excellence" goals outlined in the plan documents, aiming for very high performance, robustness, and advanced features (e.g., SIMD, NUMA awareness, fuzzing, guard pages).

## 7. Relationship to Main Protobuf Project

*   The Perl build system directly compiles C sources from the parent `upb` and `src` directories.
*   The XS layer often mirrors the structure and logic of the CPython UPB extension.
*   Aims to provide a Perl interface that is consistent with other languages.

See the other documents in this `architecture/` directory for more details on specific aspects.
