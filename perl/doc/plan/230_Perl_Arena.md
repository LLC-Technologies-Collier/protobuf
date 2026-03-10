# Milestone 23: Perl Layer - Protobuf::Arena

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/01-arena.t`.
*   [ ] Tests in `t/01-arena.t` cover all methods of `Protobuf::Arena`.
*   [ ] Add leak tests using Test::LeakTrace to `t/01-arena.t`.
*   [ ] Add `ok_test_coverage('Protobuf::Arena')` to `t/01-arena.t`.
*   [ ] Implement `Protobuf::Arena` module (`lib/Protobuf/Arena.pm`).
*   [ ] Implement XS Bindings (`lib/Protobuf/Arena.xs`).
*   [ ] Implement typemap for `upb_Arena *` in `perl/typemap`.
*   [ ] Perl tests pass for Protobuf::Arena.
*   [ ] Refine C API for `Protobuf::Arena` (if any).
*   [ ] C layer tests for Arena still pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.