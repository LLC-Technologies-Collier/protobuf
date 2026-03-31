# Milestone 299: Serialization, Deserialization, and Unknown Fields

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-299-serialize.t`.
*   [ ] Tests in `t/04-message-299-serialize.t` cover the full encode/decode cycle using the dynamically generated Perl classes.
*   [ ] Ensure that a message parsed from a binary string correctly instantiates all fields.
*   [ ] Verify that unknown fields encountered during `parse()` are preserved and successfully re-serialized when `encode()` is called.
*   [ ] Implement an interface (e.g., a method on `Protobuf::Message`) to query or retrieve unknown field data from a message instance.
*   [ ] Handle parse errors gracefully (e.g., croak with a descriptive error message instead of crashing).
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
