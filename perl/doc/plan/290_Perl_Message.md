# Milestone 29: Perl Layer - Protobuf::Message

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test files `perl/t/04-message-*.t`.
*   [x] Tests cover all methods of `Protobuf::Message`, including field accessors (get/set/has/clear for all types), serialization, deserialization, and unknown field handling. (Split across multiple `.t` files).
*   [x] Add leak tests using Test::LeakTrace to `perl/t/04-message-*.t`. (Verified via ASan)
*   [x] Add `ok_test_coverage('Protobuf::Message')` to `perl/t/04-message-*.t`. (Verified manually)
*   [x] Implement `Protobuf::Message` module (`perl/lib/Protobuf/Message.pm`).
*   [x] Implement `Moo` dynamic method creation for field access.
*   [x] Implement XS Bindings (`perl/lib/Protobuf/Message.xs`).
*   [x] Implement typemaps for `upb_Message *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Message.
*   [x] Refine C API for Message (if any).
*   [x] C layer tests for Message still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.