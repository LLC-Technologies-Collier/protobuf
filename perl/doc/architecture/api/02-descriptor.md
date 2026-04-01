# Descriptor API

_Status: C Wrappers Implemented_

The `Protobuf::Descriptor` class hierarchy in Perl (e.g., `Protobuf::MessageDescriptor`, `Protobuf::EnumDescriptor`) provides access to the underlying Protocol Buffer definitions using the `upb` library.

The C layer implementation (Milestone 6 & 7) provides a consistent set of wrapper functions for all `upb_Def*` types.

## Object Identity and Lifetime

To ensure memory safety and idiomatic Perl behavior, descriptors follow these rules:

1.  **Strict Identity**: Multiple retrievals of the same underlying `upb_Def` (e.g. `upb_MessageDef`) MUST return the same Perl SV. This is implemented via the `PerlUpb_ObjCache`.
2.  **DescriptorPool Pinning**: Every descriptor object MUST hold a **strong reference** to its parent `Protobuf::DescriptorPool` SV. Since `upb` descriptors are owned by the `upb_DefPool`, destroying the pool while a descriptor is still in use in Perl would lead to a use-after-free. Pinning ensures the C memory remains valid.
3.  **Cross-Pool Resolution**: Descriptors originating from different `DescriptorPool` instances are considered distinct, even if they represent the same proto message. Comparison logic should account for pool identity.

## C Wrapper Layer

All descriptor wrappers are located in `perl/xs/descriptor/` and follow a consistent naming convention: `PerlUpb_<Type>Def_<Method>`.

Example: `PerlUpb_MessageDef_FullName(pTHX_ const upb_MessageDef *m)`

### Implemented Wrappers

-   **FileDef (`file.c`):** Accessors for name, package, dependencies, and top-level definitions (messages, enums, extensions, services).
-   **MessageDef (`message.c`):** Accessors for name, fields (by name, number, or index), oneofs, nested types, and containing type.
-   **FieldDef (`field.c`):** Accessors for type, label, number, presence, and sub-types (message or enum).
-   **EnumDef (`enum.c`):** Accessors for name and enum values.
-   **EnumValueDef (`enum_value.c`):** Accessors for name, number, and index.
-   **OneofDef (`oneof.c`):** Accessors for name, fields, and containing type.
-   **ServiceDef (`service.c`):** Accessors for name and methods.
-   **MethodDef (`method.c`):** Accessors for name, input/output types, and streaming flags.

## Perl Attributes (Proposed)

-   **`_upb_def`**: Opaque pointer (stored as IV) to the C `upb` definition.
-   **`_pool`**: A strong Perl reference to the creating `Protobuf::DescriptorPool` to ensure its lifetime.

## Perl Methods (Proposed)

Perl methods will call the corresponding C wrappers. Methods that return other descriptors (e.g., `field->message_type()`) MUST use the `Protobuf::DescriptorPool` object cache to ensure Perl object identity and efficient memory management.

### Cache Strategy

When a wrapper returns a `upb_Def*` pointer, the Perl layer should:
1. Check the `DescriptorPool` object cache for an existing Perl wrapper for that pointer.
2. If found, return the existing Perl object.
3. If not found, create a new Perl object, register it in the cache (using a weak reference in the cache), and return it.
