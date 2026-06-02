# Milestone 294: Sub-Message Field Accessors & Mutators

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-294-submessage.t`.
*   [x] Tests in `perl/t/04-message-294-submessage.t` cover retrieving, setting, and checking presence of nested message fields.
*   [x] Ensure that retrieving an unset message field returns `undef` (implemented in `perl/xs/convert/upb_to_sv.c`).
*   [x] Ensure that retrieving an initialized message field returns a correctly blessed message instance (handled by `PerlUpb_WrapMessage` in `perl/xs/protobuf/message.c`).
*   [x] The returned sub-message instance MUST share the `upb_Arena` of the parent message (verified in `perl/xs/protobuf/message.c`).
*   [x] Implement the `has_` and `clear_` logic for sub-message fields (in `perl/xs/message/access.c`).
*   [x] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
