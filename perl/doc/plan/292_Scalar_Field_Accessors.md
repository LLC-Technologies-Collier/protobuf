# Milestone 292: Scalar Field Accessors (Getters)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-292-getters.t`.
*   [ ] Tests in `t/04-message-292-getters.t` cover retrieving values for all scalar types (int32, int64, string, bool, bytes, float, double, etc.) via dynamic accessors.
*   [ ] Update the class generator to inject reader methods for each scalar field defined in the `upb_MessageDef`.
*   [ ] Implement the underlying XS code to fetch a scalar field value from the `upb_Message` and convert it into a correctly typed Perl SV.
*   [ ] Ensure that fetching an unset optional field returns the appropriate default value defined by the protocol buffer specification.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
