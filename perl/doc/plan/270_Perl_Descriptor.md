# Milestone 27: Perl Layer - Protobuf::Descriptor & Subclasses

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/03-descriptors.t`.
*   [x] Tests in `perl/t/03-descriptors.t` cover all methods of `Protobuf::Descriptor` subclasses (MessageDef, Field, Enum, etc.).
*   [x] Add leak tests using Test::LeakTrace to `perl/t/03-descriptors.t`. (Verified via ASan).
*   [x] Add `ok_test_coverage()` checks for all descriptor subclasses in `perl/t/03-descriptors.t`. (Verified manually).
*   [ ] Implement `Protobuf::Descriptor` base module (Shared logic if needed).
*   [x] Implement subclasses (MessageDef, Field, Enum, etc.) in `perl/lib/Protobuf/Descriptor/`.
*   [ ] TODO: Implement Direct-to-Native (C-Layer) Reflection. (Difficulty: 5/10).
*   [ ] TODO: Implement Descriptor-Level Memory Profile reporting. (Difficulty: 4/10).
*   [ ] TODO: Implement Proto3-to-Proto2 Compatibility Layer. (Difficulty: 6/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement SIMD-Accelerated Descriptor Name Hashing (SSE4.2). (Difficulty: 7/10).
    *   [ ] TODO: Implement Zero-Copy Descriptor Metadata Access for high-speed inspection. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing Descriptor Identity for stable object tracking. (Difficulty: 6/10).
*   [x] Implement XS Bindings for each subclass in `perl/lib/Protobuf/Descriptor/`.
*   [x] Implement typemaps for `upb_Def*` types in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Descriptor.
*   [x] Refine C API for Descriptor (if any).
*   [x] C layer tests for Descriptor still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.