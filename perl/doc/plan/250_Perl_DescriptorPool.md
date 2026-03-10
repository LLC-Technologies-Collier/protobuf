# Milestone 25: Perl Layer - Protobuf::DescriptorPool

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/02-descriptor_pool.t`.
*   [ ] Tests in `t/02-descriptor_pool.t` cover all methods of `Protobuf::DescriptorPool` including loading descriptors, finding definitions, and message creation.
*   [ ] Add leak tests using Test::LeakTrace to `t/02-descriptor_pool.t`.
*   [ ] Add `ok_test_coverage('Protobuf::DescriptorPool')` to `t/02-descriptor_pool.t`.
*   [ ] Implement `Protobuf::DescriptorPool` module (`lib/Protobuf/DescriptorPool.pm`).
*   [ ] Implement XS Bindings (`lib/Protobuf/DescriptorPool.xs`).
*   [ ] Perl tests pass for Protobuf::DescriptorPool.
*   [ ] Refine C API for DescriptorPool (if any).
*   [ ] C layer tests for DescriptorPool still pass.
*   [ ] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.