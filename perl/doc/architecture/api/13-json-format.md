# JSON Format

_Status: Fully Implemented_

The JSON implementation in Perl provides standard-compliant Protocol Buffer JSON serialization and deserialization using the `upb` library.

## Implementation

The implementation is integrated into `Protobuf::Message`.

- **`to_json()`**: Calls the C-layer `upb_JsonEncode` function to generate a JSON representation of the message.
- **`from_json()`**: Calls the C-layer `upb_JsonDecode` function to parse JSON into a message instance.

## Advanced Performance Goals

To achieve world-class high-throughput JSON processing, the implementation includes (or is planned to include) the following:

- **Direct JSON-to-Wire Conversion**: (Planned) Implement a high-performance C-level integration that can parse JSON data directly into a wire-format serialized blob, bypassing intermediate Perl objects.
- **Streaming JSON Serialization**: (Planned) Implement a C-level JSON generator that can stream message data directly to a Perl file handle or socket, significantly reducing memory overhead for large responses.
- **JSON Schema Mapping Verification**: (Planned) Provide a utility to verify that the JSON representation of a message remains consistent with its JSON-schema definition during schema evolution.

## Memory Management

- JSON generation currently allocates a temporary buffer in the C layer, which is then copied into a Perl SV.
- Direct-to-stream printing will eliminate this intermediate copy.
