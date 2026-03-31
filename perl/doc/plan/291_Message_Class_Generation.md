# Milestone 291: Message Class Generation & Moo Setup

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-291-class.t`.
*   [x] Tests in `t/04-message-291-class.t` verify that loading a descriptor pool correctly triggers the dynamic generation of `Moo` classes for all messages in the loaded files.
*   [x] Implement a dynamic class generation mechanism (e.g., using `Moo::Role` or direct symbol table injection) that runs when `Protobuf::DescriptorPool::add_serialized_file` or similar is called.
*   [x] Ensure the generated class correctly inherits from `Protobuf::Message` or consumes the appropriate roles.
*   [x] Ensure the base `Protobuf::Message` constructor `new()` properly allocates a `upb_Message` on the provided (or newly created) `upb_Arena` and associates it with the correct `upb_MessageDef`.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
