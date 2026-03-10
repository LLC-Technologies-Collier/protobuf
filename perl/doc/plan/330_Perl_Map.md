# Milestone 33: Perl Layer - Protobuf::Map

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/06-map.t`.
*   [ ] Tests in `t/06-map.t` cover all methods of `Protobuf::Map` for various key and value types (scalar and message).
*   [ ] Add leak tests using Test::LeakTrace to `t/06-map.t`.
*   [ ] Add `ok_test_coverage('Protobuf::Map')` to `t/06-map.t`.
*   [ ] Implement `Protobuf::Map` module (`lib/Protobuf/Map.pm`).
*   [ ] Implement XS Bindings (`lib/Protobuf/Map.xs`).
*   [ ] Implement typemap for `upb_Map *` in `perl/typemap`.
*   [ ] Perl tests pass for Protobuf::Map.
*   [ ] Refine C API for Map (if any).
*   [ ] C layer tests for Map still pass.
*   [ ] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.