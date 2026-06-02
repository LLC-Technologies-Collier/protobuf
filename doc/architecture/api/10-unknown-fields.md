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

## Advanced Features and Performance

To achieve world-class observability and flexibility, the implementation includes (or is planned to include) the following:

-   **Direct Unknown-to-Message Conversion**: (Planned) Implement C-level logic to attempt parsing existing unknown field blobs into a newly provided `MessageDef`. This facilitates "lazy schema upgrades" where previously unrecognized data can be reified into typed fields.
-   **Unknown Field Indexing**: (Planned) For high-frequency tag analysis, implement internal C-level indexing of the unknown buffer to allow O(1) existence checks and retrieval of specific unknown tags.
-   **Trace Auditing**: (Planned) Integrate with the core trace/audit log to record discovery of unknown fields, aiding in identifying version drift in distributed systems.

## Memory Management

*   The `UnknownFieldSet` object holds a strong reference to the parent message's Perl SV.
*   This ensures the underlying `upb_Message` and its arena stay alive.
