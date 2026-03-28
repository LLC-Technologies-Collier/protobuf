# Architecture: Perl API Design

[TOC]

## Object-Oriented System

1.  **Moo:** The Perl modules will be implemented using **Moo**. This provides a lightweight, modern OO system with good performance and a gentle learning curve.
2.  **Immutability:** Where appropriate, attributes should be read-only (`ro`).
3.  **Type Constraints:** Moo's type constraints should be used for attribute validation where beneficial.

## Naming Conventions

1.  **Methods:** snake_case (e.g., `get_field_value`, `serialize_to_string`).
2.  **Attributes:** snake_case.
3.  **Package Names:** CamelCase (e.g., `Protobuf::Message`).

## Method Styles

1.  **Accessors:** Use standard Moo `has` for attribute access.
2.  **Actions:** Methods performing actions should be named descriptively (e.g., `parse`, `serialize`, `add_item`).
