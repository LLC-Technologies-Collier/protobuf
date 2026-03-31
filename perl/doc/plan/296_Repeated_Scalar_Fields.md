# Milestone 296: Repeated Field Accessors (Scalars)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-296-repeated-scalar.t`.
*   [ ] Tests in `t/04-message-296-repeated-scalar.t` cover retrieving, setting, and clearing repeated scalar fields.
*   [ ] Implement a lightweight tied array class `Protobuf::RepeatedScalarField` (or similar) that acts as a proxy for the underlying `upb_Array`.
*   [ ] Update the class generator to return this tied array (or a reference to it) when a repeated scalar field is accessed.
*   [ ] Ensure standard Perl array operations (e.g., `push`, `pop`, `shift`, `unshift`, `splice`, scalar assignment, size fetching) work seamlessly and update the underlying UPB data structure without needing a manual "save" step.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
