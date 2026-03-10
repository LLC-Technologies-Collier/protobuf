# Descriptor API

_Status: Not Started_

The `Protobuf::Descriptor` class is a Perl wrapper around various `upb_Def*` types, such as `upb_MessageDef`, `upb_EnumDef`, `upb_FieldDef`, etc.

## Attributes

-   **`_upb_def`**: Opaque pointer to the C definition.
-   **`_type`**: String indicating the definition type (e.g., 'message', 'enum', 'field').
-   **`_pool`**: A strong Perl reference to the creating `Protobuf::DescriptorPool`.

## Methods

Methods will expose attributes of the underlying `upb` definition. Examples for a Message Descriptor:

-   `name()`: Returns `upb_MessageDef_Name()`.
-   `full_name()`: Returns `upb_MessageDef_FullName()`.
-   `syntax()`: Returns string form of `upb_MessageDef_Syntax()`.
-   `file()`: Returns a `Protobuf::FileDescriptor` (requiring cache lookup).
-   `fields()`: Returns a list of `Protobuf::FieldDescriptor`s.
-   `oneofs()`: Returns a list of `Protobuf::OneofDescriptor`s.
-   `containing_type()`: Returns a `Protobuf::Descriptor` or undef.
-   `nested_types()`: List of `Protobuf::Descriptor`s.
-   `enum_types()`: List of `Protobuf::EnumDescriptor`s.

All methods returning Descriptor objects must use the pool's object cache.
