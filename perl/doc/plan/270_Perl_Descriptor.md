# Milestone 27: Perl Layer - Protobuf::Descriptor & Subclasses

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/03-descriptor.t`.
*   [ ] Tests in `t/03-descriptor.t` cover all methods of `Protobuf::Descriptor` and its subclasses (Message, Field, Enum, Service, etc.).
*   [ ] Add leak tests using Test::LeakTrace to `t/03-descriptor.t`.
*   [ ] Add `ok_test_coverage()` checks for `Protobuf::Descriptor` and all its subclasses in `t/03-descriptor.t`.
*   [ ] Implement `Protobuf::Descriptor` base module (`lib/Protobuf/Descriptor.pm`).
*   [ ] Implement subclasses (Message, Field, Enum, Service, etc.) in `lib/Protobuf/Descriptor/`.
*   [ ] Implement XS Bindings (`lib/Protobuf/Descriptor.xs`).
*   [ ] Implement typemaps for `upb_Def*` types in `perl/typemap`.
*   [ ] Perl tests pass for Protobuf::Descriptor.
*   [ ] Refine C API for Descriptor (if any).
*   [ ] C layer tests for Descriptor still pass.
*   [ ] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.