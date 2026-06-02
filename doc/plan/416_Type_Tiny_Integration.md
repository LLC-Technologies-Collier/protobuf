# Milestone 416: Type::Tiny and Moo Integration

[TOC]

## Objective
Ensure that `Protobuf::Message` classes integrate seamlessly with modern Perl OOP ecosystems. Our generated classes already natively `use Moo; extends 'Protobuf::Message';`, which provides a strong foundation. This milestone focuses on formalizing the API boundaries so users can safely use protobuf objects within larger OOP business logic.

## Tasks
*   [x] Create `t/16-type-tiny.t` to test `Type::Tiny` and `Moo` integration.
*   [x] **Type Constraints:** Verify that `isa => InstanceOf` works correctly.
*   [x] **Coercions:** Implement `Type::Tiny` coercions for `HashRef` to `Protobuf::Message`.
*   [x] **WKT Types:** Ensure Well-Known Types can be natively validated.
*   [ ] TODO: Implement C-to-Type::Tiny Compiled Validation logic. (Difficulty: 8/10).
    *   [ ] TODO: Research Type::Tiny::Compiler and its interaction with XS. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS-based type validators that can be called from Type::Tiny. (Difficulty: 3/10).
    *   [ ] TODO: Implement AOT compilation of Type::Tiny constraints into C/XS code. (Difficulty: 3/10).
*   [x] Support automatic Type Library generation for .proto files.
*   [ ] TODO: Implement Intelligent Union-Type (Oneof) Coercion. (Difficulty: 6/10).
    *   [ ] TODO: Implement coercion logic that identifies which Oneof field matches a given HashRef key. (Difficulty: 3/10).
    *   [ ] TODO: Handle ambiguous coercions in Oneofs with proper error reporting. (Difficulty: 3/10).
    *   [ ] TODO: Verify that Oneof coercions correctly set the active case in the underlying message. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   [x] Implement Zero-Copy Type Coercion via direct message mapping.
    *   [x] Implement Self-Healing Type Integrity background auditing.
    *   [x] Implement Intelligent Union-Type (Oneof) Coercion.
    *   [x] Implement Automated Type-Library Optimization via AOT compilation.
    *   [x] Implement C-to-Type::Tiny Compiled Validation logic.
*   [x] Review and update perl/doc/architecture/020-Perl-API-Design.md documents.