# Milestone 29: Perl Layer - Protobuf::Message

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message.t`.
*   [ ] Tests in `t/04-message.t` cover all methods of `Protobuf::Message`, including field accessors (get/set/has/clear for all types), serialization, deserialization, and unknown field handling.
*   [ ] Add leak tests using Test::LeakTrace to `t/04-message.t`.
*   [ ] Add `ok_test_coverage('Protobuf::Message')` to `t/04-message.t`.
*   [ ] Implement `Protobuf::Message` module (`lib/Protobuf/Message.pm`).
*   [ ] Implement `AUTOLOAD` for field access.
*   [ ] Implement XS Bindings (`lib/Protobuf/Message.xs`).
*   [ ] Implement typemaps for `upb_Message *` in `perl/typemap`.
*   [ ] Perl tests pass for Protobuf::Message.
*   [ ] Refine C API for Message (if any).
*   [ ] C layer tests for Message still pass.
*   [ ] All prior Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.