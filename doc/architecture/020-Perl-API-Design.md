# Architecture: Perl API Design

[TOC]

## Object-Oriented System

1.  **Moo:** The Perl modules will be implemented using **Moo**. This provides a lightweight, modern OO system with good performance and a gentle learning curve.
2.  **Immutability:** Where appropriate, attributes should be read-only (`ro`).
3.  **Type Constraints:** Moo's type constraints should be used for attribute validation where beneficial.
4.  **Type::Tiny Integration**: (Planned) Seamless integration with `Type::Tiny` for rigorous message and field validation, including automatic generation of type libraries for every Protobuf package.
5.  **Compiled Validation**: (Planned) High-performance C-level validation logic is exported as compiled Perl subroutines, allowing `Type::Tiny` to perform complex checks without full object inflation.
6.  **Intelligent Coercion**: (Planned) Support for deep, intelligent coercion of complex Perl `HashRefs` into nested Protobuf message trees, including automated `oneof` branch selection.

## Naming Conventions

1.  **Methods:** snake_case (e.g., `get_field_value`, `serialize_to_string`).
2.  **Attributes:** snake_case.
3.  **Package Names:** CamelCase (e.g., `Protobuf::Message`).

## Method Styles

1.  **Accessors:** Use standard Moo `has` for attribute access.
2.  **Actions:** Methods performing actions should be named descriptively (e.g., `parse`, `serialize`, `add_item`).

## Future Considerations

1.  **Native Booleans:** Perl has added a native boolean type. Consider how this might be integrated into the API for boolean fields, potentially using `boolean.pm` or similar, for stricter type checking and clarity, instead of relying solely on Perl's standard truthiness.
2.  **Asynchronous Operations:** Explore integration with `IO::Async` or other event loops like `Mojo::IOLoop` to provide non-blocking APIs for operations like service calls or streaming, especially relevant for network-bound Protobuf usage.
