# Milestone 297: Repeated Field Accessors (Messages)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Create test file `perl/t/04-message-297-repeated-message.t`.
*   [x] Tests in `perl/t/04-message-297-repeated-message.t` cover retrieving, setting, and clearing repeated message fields.
*   [x] Use the tied array class `Protobuf::Internal::Repeated` (shared with scalars) to handle sub-messages.
*   [x] Ensure that retrieving an item from the array returns a correctly blessed message instance sharing the parent's `upb_Arena` (handled in `perl/xs/repeated/repeated.c`).
*   [x] Handle adding new messages via plain HashRefs (auto-populating) or existing message objects (deep-copying) in `perl/xs/convert/sv_to_upb.c`.
*   [x] Ensure standard Perl array operations (`push`, index assignment, etc.) work correctly with sub-messages.
*   [x] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.

