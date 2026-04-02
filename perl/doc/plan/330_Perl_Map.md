# Milestone 33: Perl Layer - Protobuf::Map

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/06-map.t`.
*   [x] Tests in `t/06-map.t` cover all methods of `Protobuf::Map` for various key and value types (scalar and message).
*   [ ] Add leak tests using Test::LeakTrace to `t/06-map.t`.
*   [ ] Add `ok_test_coverage('Protobuf::Map')` to `t/06-map.t`.
*   [x] Implement `Protobuf::Map` module (`lib/Protobuf/Map.pm`).
*   [x] Implement O(1) Bulk Map Projection. (Skeletal implementation).
*   [ ] TODO: Implement Shared-Arena Key Deduplication.
*   [ ] TODO: Provide Real-Time Map Collision Analysis API.
*   [x] Implement XS Bindings (`lib/Protobuf/Map.xs`).
*   [ ] Implement typemap for `upb_Map *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Map.
*   [x] Refine C API for Map (if any).
*   [x] C layer tests for Map still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.