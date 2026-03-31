# Milestone 294: Sub-Message Field Accessors & Mutators

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-294-submessage.t`.
*   [x] Tests in `t/04-message-294-submessage.t` cover retrieving, setting, and checking presence of nested message fields.
*   [x] Ensure that retrieving an unset message field returns `undef` (or similar spec-compliant behavior) rather than instantiating an empty message automatically.
*   [x] Ensure that retrieving an initialized message field returns a correctly blessed `Protobuf::Message` instance.
*   [x] The returned sub-message instance MUST share the `upb_Arena` of the parent message to prevent memory corruption and ensure correct lifetime management.
*   [x] Implement the `has_` and `clear_` logic for sub-message fields.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
