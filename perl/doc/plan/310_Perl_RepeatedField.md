# Milestone 31: Perl Layer - Protobuf::RepeatedField

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/05-repeated.t`.
*   [ ] Tests in `t/05-repeated.t` cover all methods of `Protobuf::RepeatedField` for various scalar and message types.
*   [ ] Add leak tests using Test::LeakTrace to `t/05-repeated.t`.
*   [ ] Add `ok_test_coverage('Protobuf::RepeatedField')` to `t/05-repeated.t`.
*   [ ] Implement `Protobuf::RepeatedField` module (`lib/Protobuf/RepeatedField.pm`).
*   [ ] Implement XS Bindings (`lib/Protobuf/RepeatedField.xs`).
*   [ ] Implement typemap for `upb_Array *` in `perl/typemap`.
*   [ ] Perl tests pass for Protobuf::RepeatedField.
*   [ ] Refine C API for RepeatedField (if any).
*   [ ] C layer tests for RepeatedField still pass.
*   [ ] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.