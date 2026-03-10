# Message Field Accessors

_Status: Not Started_

Field access in `Protobuf::Message` objects is designed to be idiomatic Perl.

## `AUTOLOAD` Mechanism

-   Getters (`$message->field_name()`), setters (`$message->set_field_name($value)`), presence checkers (`$message->has_field_name()`), and clearers (`$message->clear_field_name()`) are handled by `AUTOLOAD` in `Protobuf::Message`.
-   `AUTOLOAD` parses the method name to determine the field name and operation type.
-   It looks up the `upb_FieldDef` from the message's `_descriptor`.
-   It calls a generic XS dispatcher function, passing the operation type, `upb_Message*`, and `upb_FieldDef*`.

## XS Dispatch Functions

-   A central XS function (e.g., `perl_upb_msg_field_access`) takes the Perl `self` object, field name, operation type, and optional values.
-   It retrieves the `upb_Message*` and `upb_MiniTable*`.
-   Handles stub reification if necessary (for message fields).
-   Uses the `upb/message/accessors.h` functions (`upb_Message_Get...`, `upb_Message_Set...`, etc.) to perform the operation.
-   Converts results back to Perl SVs using functions from `xs/types.c`, utilizing the object cache for message types.

## Type Handling

-   The XS layer uses the `upb_FieldDef` to determine the field's C type and uses the appropriate type conversion functions.
-   See [Type Conversion](core/05-type-conversion.md).
