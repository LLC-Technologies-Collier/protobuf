# Milestone 296: Repeated Field Accessors (Scalars)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Create test file `perl/t/04-message-296-repeated-scalar.t`.
*   [x] Tests in `perl/t/04-message-296-repeated-scalar.t` cover retrieving, setting, and clearing repeated scalar fields.
*   [x] Implement a lightweight tied array class `Protobuf::Internal::Repeated` (in `perl/lib/Protobuf/Internal/Repeated.pm` and `perl/lib/Protobuf/Internal/Repeated.xs`) that acts as a proxy for the underlying `upb_Array`.
*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to return this tied array when a repeated scalar field is accessed.
*   [x] Ensure standard Perl array operations (e.g., `push`, `pop`, `shift`, `unshift`, `splice`, scalar assignment, size fetching) work seamlessly and update the underlying UPB data structure without needing a manual "save" step.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.

