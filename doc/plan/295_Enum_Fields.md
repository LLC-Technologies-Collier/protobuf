# Milestone 295: Enum Field Handling

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-295-enum.t`.
*   [x] Tests in `perl/t/04-message-295-enum.t` cover retrieving and setting Enum fields.
*   [x] Implement logic to allow setting Enum fields using both the integer value and the string name (implemented in `perl/xs/convert/sv_to_upb.c`).
*   [x] Validate that setting an invalid Enum value appropriately throws a croak/exception.
*   [x] Implement the logic to retrieve the Enum value (currently returns integer in `perl/xs/convert/upb_to_sv.c`).
*   [x] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
