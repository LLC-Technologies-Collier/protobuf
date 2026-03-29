# Unknown Fields

_Status: C Layer Implemented_

Unknown fields are fields encountered during parsing that are not recognized by the message definition. UPB preserves these fields in the message's internal buffer.

## Implementation

The C implementation is located in `perl/xs/unknown_fields/`.

*   **`set.c`**: Implements the `Protobuf::UnknownFieldSet` wrapper.
    *   `PerlUpb_UnknownFieldSet_New`: Creates a new wrapper for a given message.
    *   `PerlUpb_UnknownFieldSet_GetData`: Concatenates all unknown field segments into a single Perl string SV. Uses `upb_Message_NextUnknown` for iteration.
    *   `PerlUpb_UnknownFieldSet_Add`: Appends raw bytes to the unknown field buffer using the internal `_upb_Message_AddUnknown` API.
    *   `PerlUpb_UnknownFieldSet_Clear`: Discards all unknown fields using `_upb_Message_DiscardUnknown_shallow`.

## Perl Interface

In Perl, `Protobuf::UnknownFieldSet` will provide access to the raw data:

```perl
my $unknown_data = $msg->unknown_fields->data;
$msg->unknown_fields->add($raw_bytes);
$msg->unknown_fields->clear;
```

## Memory Management

*   The `UnknownFieldSet` object holds a strong reference to the parent message's Perl SV.
*   This ensures the underlying `upb_Message` and its arena stay alive.
