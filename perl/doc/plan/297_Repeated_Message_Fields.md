# Milestone 297: Repeated Field Accessors (Messages)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-297-repeated-message.t`.
*   [ ] Tests in `t/04-message-297-repeated-message.t` cover retrieving, setting, and clearing repeated message fields.
*   [ ] Extend the tied array class `Protobuf::RepeatedMessageField` (or similar) to handle sub-messages.
*   [ ] Ensure that retrieving an item from the array returns a correctly blessed `Protobuf::Message` instance sharing the parent's `upb_Arena`.
*   [ ] Determine the behavior for adding a new message to the repeated field. Does the user pass in a fully formed message, or a hashref of data that is automatically converted into a message allocated on the arena? Implement this behavior.
*   [ ] Ensure standard Perl array operations (`push`, index assignment, etc.) work correctly with sub-messages.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
