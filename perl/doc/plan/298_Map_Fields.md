# Milestone 298: Map Field Accessors

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-298-map.t`.
*   [x] Tests in `t/04-message-298-map.t` cover retrieving, setting, and clearing map fields.
*   [x] Flesh out the tied hash class `Protobuf::Internal::Map` to support all standard Perl hash operations:
    *   [x] `FETCH`
    *   [x] `STORE`
    *   [x] `DELETE`
    *   [x] `CLEAR`
    *   [x] `EXISTS`
    *   [x] `FIRSTKEY`
    *   [x] `NEXTKEY`
    *   [x] `SCALAR`
*   [x] Implement `Protobuf::Internal::MapIterator` XS methods to support hash iteration.
*   [x] Ensure that retrieving a value from the map returns a correctly blessed `Protobuf::Message` instance (if the value type is a message) or the correct scalar type.
*   [x] Ensure that setting a value in the map performs correct type-checking and conversion.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
