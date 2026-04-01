# Milestone 416: Type::Tiny and Moo Integration

[TOC]

## Objective
Ensure that `Protobuf::Message` classes integrate seamlessly with modern Perl OOP ecosystems. Our generated classes already natively `use Moo; extends 'Protobuf::Message';`, which provides a strong foundation. This milestone focuses on formalizing the API boundaries so users can safely use protobuf objects within larger OOP business logic.

## Tasks
*   [x] Create `t/16-type-tiny.t` to test `Type::Tiny` and `Moo` integration.
*   [x] **Type Constraints:** Verify that `isa => InstanceOf['my_package::MyMessage']` works correctly inside custom user Moo classes without requiring extra wrappers.
*   [x] **Coercions:** Implement and document `Type::Tiny` coercions allowing raw standard Perl `HashRef` objects to be automatically coerced into instantiated `Protobuf::Message` objects (via `from_perl()`) during Moo object construction.
*   [x] **WKT Types:** Ensure Well-Known Types (like `google.protobuf.Any` and `google.protobuf.Struct`) can be natively validated using `Types::Standard` where appropriate.
*   [ ] TODO: Implement C-to-Type::Tiny Compiled Validation logic.
*   [ ] TODO: Support automatic Type Library generation for .proto files.
*   [ ] TODO: Implement Intelligent Union-Type (Oneof) Coercion.
*   [x] Review and update perl/doc/architecture/020-Perl-API-Design.md documents.