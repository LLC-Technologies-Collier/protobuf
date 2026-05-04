# Milestone 500: Pure-Perl "Polyglot" Engine Support

[TOC]

## Objective

Implement a complete Pure-Perl fallback for the Protocol Buffers engine. This provides "Zero-Dependency" runtime support (no C compiler required at install time for users who only need the PP engine) and allows for a "Polyglot" mode where specific messages can opt into Pure-Perl for write-heavy workloads that suffer from XS transition overhead.

## Architecture

The Pure-Perl engine will mirror the behavior of the UPB-based XS engine. It will utilize a `Protobuf::Engine::PurePerl` backend that handles data storage, serialization, and reflection.

### 1. Engine Abstraction
*   [x] Refactor `Protobuf::Message` to support swappable backends.
*   [x] Implement `Protobuf::Engine::XS` (current default).
*   [x] Implement `Protobuf::Engine::PurePerl` (new).
*   [x] Add global `PROTOBUF_ENGINE` environment variable and per-message `profile => 'pure_perl'` support.

### 2. Pure-Perl Descriptor Reflection
*   [x] Implement a Perl-only version of the `DescriptorPool`.
*   [x] Create a parser for binary `FileDescriptorSet` blobs in Perl (to load embedded descriptors without UPB).
*   [x] Implement Perl classes for `MessageDef`, `FieldDef`, `EnumDef`, etc., with the same API as the XS descriptors.

### 3. Wire Format Implementation (G::PB Style)
*   [x] **Varint Kernels**: Implement `encode_varint` and `decode_varint` using Perl arithmetic and bitwise ops.
*   [x] **Fixed Types**: Utilize Perl's `pack`/`unpack` (templates `V`, `v`, `Q<`, `d`, `f`) for fixed32, fixed64, doubles, and floats.
*   [x] **ZigZag**: Implement ZigZag encoding/decoding for signed integers.
*   [x] **Parsing**: Implement the tag-based parse loop in Perl.
*   [x] **Serialization**: Implement the recursive serialization tree traversal.

### 4. Data Storage & Accessors
*   [x] Implement a Hash-backed storage model for PP messages.
*   [x] Ensure the `ClassGenerator` can inject Pure-Perl accessors that bypass XS when the PP engine is active (Achieved via engine-aware XS fallback and Perl-only generation).
*   [x] Implement `has_`, `clear_`, and default value logic in Perl.

### 5. Parity & Validation
*   [x] Pass all 318 existing tests using the Pure-Perl engine (where applicable) and verify XS stability.
*   [x] Verify "Polyglot" stability: An XS-backed message containing a PP-backed sub-message (and vice-versa).
*   [ ] Implement "Zero-Copy Bridge": Logic to convert an XS message to a PP message (and vice-versa) for cross-engine operations.

## Engineering Excellence (Reach for More)
*   [x] **Pure-Perl Reflection**: Fully operational zero-dependency reflection system.
*   **JIT Accessor Generation**: Use `eval` to generate optimized Perl accessor code for specific schemas at runtime. (Difficulty: 4/10)
*   **SIMD-lite for PP**: Research using `String::SIMD` (if available) or `pack` tricks to accelerate Varint decoding in Perl. (Difficulty: 6/10)
*   **Memory Efficiency**: Implement an array-backed storage model for Pure-Perl messages to reduce memory footprint compared to Hashes. (Difficulty: 5/10)
*   **G::PB Bridge**: Implement a compatibility layer that allows `Google::ProtocolBuffers` generated code to run directly on the new PP engine. (Difficulty: 4/10)
