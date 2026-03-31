# Milestone 293: Scalar Field Mutators (Setters, Clearers, Has)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-293-mutators.t`.
*   [ ] Tests in `t/04-message-293-mutators.t` cover setting, clearing, and checking presence (`has_`) of scalar fields.
*   [ ] Update the class generator to inject `set_$fieldname`, `clear_$fieldname`, and `has_$fieldname` methods.
*   [ ] Implement the XS logic to perform type-checking/conversion on the incoming Perl SV and write the value into the `upb_Message`.
*   [ ] Implement the XS logic to clear a field and to check if a field is explicitly set.
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
