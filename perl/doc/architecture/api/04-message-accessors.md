# Message Field Accessors

_Status: C Layer Implemented_

Field access in `Protobuf::Message` objects is designed to be idiomatic Perl.

## C Implementation

The XS-layer C functions are located in `perl/xs/message/access.c`.

*   **`PerlUpb_Message_GetField`**: Retrieves a field value.
    *   Uses `upb_Message_GetFieldByDef` to get a `upb_MessageValue`.
    *   Converts it to a Perl SV via `PerlUpb_UpbToSv`.
    *   For message types, it returns a Perl wrapper blessed into the appropriate class.
*   **`PerlUpb_Message_SetField`**: Sets a field value.
    *   Converts the Perl SV to a `upb_MessageValue` via `PerlUpb_SvToUpb`.
    *   If the field is a message, it performs a `upb_Message_DeepCopy` into the destination message's arena.
    *   Uses `upb_Message_SetFieldByDef` to store the value.
*   **`PerlUpb_Message_HasField`**: Checks if a field with presence is set.
*   **`PerlUpb_Message_ClearField`**: Clears a specific field.
*   **`PerlUpb_Message_Clear`**: Clears all fields in the message.

## Perl Layer Integration (Future)

-   Getters (`$message->field_name()`), setters (`$message->set_field_name($value)`), presence checkers (`$message->has_field_name()`), and clearers (`$message->clear_field_name()`) will be handled by `AUTOLOAD` in `Protobuf::Message`.
-   `AUTOLOAD` will look up the `upb_FieldDef` and call the corresponding C function.

## Serialization and Comparison

Implemented in `perl/xs/message/serialize.c` and `perl/xs/message/compare.c`.

*   **`PerlUpb_Message_Serialize`**: Serializes message to wire format using `upb_Encode`.
*   **`PerlUpb_Message_Parse`**: Creates a new message and arena, and parses data into it via `upb_Decode`.
*   **`PerlUpb_Message_IsEqual`**: Compares two messages for value equality using `upb_Message_IsEqual`.

## Type Handling

-   The C layer uses `upb_FieldDef` to determine the field's type and uses `PerlUpb_SvToUpb` / `PerlUpb_UpbToSv` for conversion.
-   See [Type Conversion](core/05-type-conversion.md).
