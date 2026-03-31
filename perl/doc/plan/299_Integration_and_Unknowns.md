# Milestone 299: Serialization, Deserialization, and Unknown Fields

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-299-integration.t`.
*   [ ] Tests in `t/04-message-299-integration.t` cover:
    *   `serialize()`: Convert a populated message into binary wire format.
    *   `parse()`: Create a message instance from binary wire format.
    *   `parse_from()`: Populate an existing message instance from binary wire format (merging).
    *   Unknown field preservation: Ensure that unknown fields are kept during a parse-serialize roundtrip.
    *   Accessing unknown fields: Implement `Protobuf::UnknownFieldSet` wrapper in Perl.
*   [ ] Update `Protobuf::Message` to expose `parse` as a class method and `parse_from` as an instance method.
*   [ ] Ensure `parse` correctly uses the message's generated class.
*   [ ] Implement `Protobuf::UnknownFieldSet` and expose it via `$msg->unknown_fields`.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
*   [ ] FINAL REVIEW: Ensure all Milestones 291-299 are verified and documented.
