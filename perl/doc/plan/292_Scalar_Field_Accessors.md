# Milestone 292: Scalar Field Accessors (Getters)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-292-getters.t`.
*   [x] Tests in `perl/t/04-message-292-getters.t` cover retrieving values for all scalar types (int32, int64, string, bool, bytes, float, double, etc.) via dynamic accessors.
*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to inject reader methods for each scalar field defined in the `upb_MessageDef`.
*   [x] Implement the underlying XS code (in `perl/lib/Protobuf/Message.xs`) to fetch a scalar field value from the `upb_Message`.
*   [x] Ensure that fetching an unset optional field returns the appropriate default value defined by the protocol buffer specification.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
