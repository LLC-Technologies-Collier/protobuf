# Milestone 293: Scalar Field Mutators (Setters, Clearers, Has)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-293-mutators.t`.
*   [x] Tests in `perl/t/04-message-293-mutators.t` cover setting, clearing, and checking presence (`has_`) of scalar fields.
*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to inject `set_$fieldname`, `clear_$fieldname`, and `has_$fieldname` methods.
*   [x] Implement the XS logic (in `perl/lib/Protobuf/Message.xs`) to perform type-checking/conversion on the incoming Perl SV and write the value into the `upb_Message`.
*   [x] Implement the XS logic to clear a field and to check if a field is explicitly set.
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
