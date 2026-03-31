# Milestone 294: Sub-Message Field Accessors & Mutators

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-294-submessage.t`.
*   [ ] Tests in `t/04-message-294-submessage.t` cover retrieving, setting, and checking presence of nested message fields.
*   [ ] Ensure that retrieving an unset message field returns `undef` (or similar spec-compliant behavior) rather than instantiating an empty message automatically.
*   [ ] Ensure that retrieving an initialized message field returns a correctly blessed `Protobuf::Message` instance.
*   [ ] The returned sub-message instance MUST share the `upb_Arena` of the parent message to prevent memory corruption and ensure correct lifetime management.
*   [ ] Implement the `has_` and `clear_` logic for sub-message fields.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
