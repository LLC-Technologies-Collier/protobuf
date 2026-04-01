# Milestone 299: Serialization, Deserialization, and Unknown Fields

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-299-integration.t`.
*   [x] Tests in `perl/t/04-message-299-integration.t` cover:
    *   [x] `serialize()`: Convert a populated message into binary wire format.
    *   [x] `parse()`: Create a message instance from binary wire format.
    *   [ ] `parse_from()`: Populate an existing message instance from binary wire format (merging). (TODO: Not yet implemented).
    *   [x] Unknown field preservation: Ensure that unknown fields are kept during a parse-serialize roundtrip.
    *   [x] Accessing unknown fields: Implement `Protobuf::UnknownFieldSet` wrapper in Perl.
*   [x] Update `Protobuf::Message` (in `perl/lib/Protobuf/Message.pm`) to expose `parse` as a class method.
*   [ ] Update `Protobuf::Message` to expose `parse_from` as an instance method. (TODO: Not yet implemented).
*   [x] Ensure `parse` correctly uses the message's generated class.
*   [x] Implement `Protobuf::UnknownFieldSet` (in `perl/lib/Protobuf/UnknownFieldSet.pm` and `perl/lib/Protobuf/UnknownFieldSet.xs`) and expose it via `$msg->unknown_fields`.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
*   [ ] FINAL REVIEW: Ensure all Milestones 291-299 are verified and documented.
