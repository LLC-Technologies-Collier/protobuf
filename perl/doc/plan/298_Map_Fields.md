# Milestone 298: Map Field Accessors

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-298-map.t`.
*   [ ] Tests in `t/04-message-298-map.t` cover retrieving, setting, and clearing map fields (both scalar keys/values and scalar keys/message values).
*   [ ] Implement a lightweight tied hash class `Protobuf::MapField` (or similar) that acts as a proxy for the underlying `upb_Map`.
*   [ ] Update the class generator to return this tied hash (or a reference to it) when a map field is accessed.
*   [ ] Ensure standard Perl hash operations (e.g., `keys`, `values`, `exists`, `delete`, hash assignment, value fetching) work seamlessly and update the underlying UPB data structure without needing a manual "save" step.
*   [ ] Ensure that retrieving a map value that is a sub-message returns a correctly blessed `Protobuf::Message` instance sharing the parent's `upb_Arena`.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
