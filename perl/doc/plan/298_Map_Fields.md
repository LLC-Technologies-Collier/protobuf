# Milestone 298: Map Field Accessors

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-298-map.t`.
*   [ ] Tests in `t/04-message-298-map.t` cover retrieving, setting, and clearing map fields.
*   [ ] Flesh out the tied hash class `Protobuf::Internal::Map` to support all standard Perl hash operations:
    *   `FETCH`
    *   `STORE`
    *   `DELETE`
    *   `CLEAR`
    *   `EXISTS`
    *   `FIRSTKEY`
    *   `NEXTKEY`
    *   `SCALAR`
*   [ ] Implement `Protobuf::Internal::MapIterator` XS methods to support hash iteration.
*   [ ] Ensure that retrieving a value from the map returns a correctly blessed `Protobuf::Message` instance (if the value type is a message) or the correct scalar type.
*   [ ] Ensure that setting a value in the map performs correct type-checking and conversion.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
