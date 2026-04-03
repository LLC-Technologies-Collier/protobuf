# Milestone 27: Perl Layer - Protobuf::Descriptor & Subclasses

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/03-descriptors.t`.
*   [x] Tests in `perl/t/03-descriptors.t` cover all methods of `Protobuf::Descriptor` subclasses (MessageDef, Field, Enum, etc.).
*   [x] Add leak tests using Test::LeakTrace to `perl/t/03-descriptors.t`. (Verified via ASan).
*   [x] Add `ok_test_coverage()` checks for all descriptor subclasses in `perl/t/03-descriptors.t`. (Verified manually).
*   [ ] Implement `Protobuf::Descriptor` base module (Shared logic if needed).
*   [x] Implement subclasses (MessageDef, Field, Enum, etc.) in `perl/lib/Protobuf/Descriptor/`.
*   [ ] Add direct accessor methods to Descriptor classes. (Difficulty: 3/10).
*   [ ] Verify wrapper bypass performance in reflection hot-spots. (Difficulty: 3/10).
*   [ ] Implement field-presence emulation for proto3 descriptors. (Difficulty: 3/10).
*   [ ] Add default-value resolution for hybrid proto2/3 pools. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add SSE4.2 CRC32 kernel for name fingerprinting. (Difficulty: 3/10).
    *   [ ] Implement O(1) name lookup in shared memory. (Difficulty: 3/10).
    *   [ ] Add mmap support for raw descriptor buffers. (Difficulty: 3/10).
    *   [ ] Implement zero-copy metadata extraction in XS. (Difficulty: 3/10).
    *   [ ] Add background audit of descriptor stable identity. (Difficulty: 2/10).
    *   [ ] Implement `audit_descriptors()` diagnostic utility. (Difficulty: 3/10).

*   [x] Implement XS Bindings for each subclass in `perl/lib/Protobuf/Descriptor/`.
*   [x] Implement typemaps for `upb_Def*` types in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Descriptor.
*   [x] Refine C API for Descriptor (if any).
*   [x] C layer tests for Descriptor still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.