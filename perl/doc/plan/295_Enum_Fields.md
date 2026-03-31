# Milestone 295: Enum Field Handling

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-295-enum.t`.
*   [ ] Tests in `t/04-message-295-enum.t` cover retrieving and setting Enum fields.
*   [ ] Implement logic to allow setting Enum fields using both the integer value (e.g., `1`) and the string name (e.g., `'FOO'`).
*   [ ] Validate that setting an invalid Enum value appropriately throws a croak/exception.
*   [ ] Implement the logic to retrieve the Enum value. Decide on the idiomatic Perl representation for Enum getters (e.g., returning the integer vs. returning the string name, or returning a dualvar/overloaded object).
*   [ ] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
