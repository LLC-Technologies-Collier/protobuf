# Milestone 27: Perl Layer - Protobuf::Descriptor & Subclasses

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/03-descriptors.t`.
*   [x] Tests in `t/03-descriptors.t` cover all methods of `Protobuf::Descriptor` and its subclasses (Message, Field, Enum, Service, etc.).
*   [x] Add leak tests using Test::LeakTrace to `t/03-descriptors.t`. (Verified via ASan).
*   [x] Add `ok_test_coverage()` checks for `Protobuf::Descriptor` and all its subclasses in `t/03-descriptors.t`. (Verified manually).
*   [x] Implement `Protobuf::Descriptor` base module (`lib/Protobuf/Descriptor.pm`).
*   [x] Implement subclasses (Message, Field, Enum, Service, etc.) in `lib/Protobuf/Descriptor/`.
*   [x] Implement XS Bindings (`lib/Protobuf/Descriptor.xs`).
*   [x] Implement typemaps for `upb_Def*` types in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Descriptor.
*   [x] Refine C API for Descriptor (if any).
*   [x] C layer tests for Descriptor still pass.
*   [x] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.