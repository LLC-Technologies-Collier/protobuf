# Unknown Fields

_Status: Fully Implemented_

Unknown fields are fields encountered during parsing that are not recognized by the message definition. UPB preserves these fields in the message's internal buffer.

## Implementation

### C Layer
The C implementation is located in `perl/xs/unknown_fields/`.
...
    *   `PerlUpb_UnknownFieldSet_Clear`: Discards all unknown fields using `_upb_Message_DiscardUnknown_shallow`.

### Perl Layer
The Perl implementation is located in `perl/lib/Protobuf/UnknownFieldSet.pm` and `perl/lib/Protobuf/UnknownFieldSet.xs`.

## Perl Interface
...

In Perl, `Protobuf::UnknownFieldSet` will provide access to the raw data:

```perl
my $unknown_data = $msg->unknown_fields->data;
$msg->unknown_fields->add($raw_bytes);
$msg->unknown_fields->clear;
```

## Memory Management

*   The `UnknownFieldSet` object holds a strong reference to the parent message's Perl SV.
*   This ensures the underlying `upb_Message` and its arena stay alive.
