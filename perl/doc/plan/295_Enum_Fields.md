# Milestone 295: Enum Field Handling

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-295-enum.t`.
*   [x] Tests in `t/04-message-295-enum.t` cover retrieving and setting Enum fields.
*   [x] Implement logic to allow setting Enum fields using both the integer value (e.g., `1`) and the string name (e.g., `'FOO'`).
*   [x] Validate that setting an invalid Enum value appropriately throws a croak/exception.
*   [x] Implement the logic to retrieve the Enum value. Decide on the idiomatic Perl representation for Enum getters (e.g., returning the integer vs. returning the string name, or returning a dualvar/overloaded object).
*   [x] Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
