# TextFormat

_Status: Fully Implemented_

The `TextFormat` implementation in Perl provides a human-readable representation of Protocol Buffer messages, primarily for debugging and configuration.

## Implementation

The implementation is integrated into `Protobuf::Message`.

- **`to_text()`**: Calls the C-layer `upb_TextEncode` function to generate a string representation of the message.
- **Parsing**: (Planned) Support for parsing TextFormat data into message instances using `upb_TextDecode`.

## Advanced Performance Goals

To achieve world-class observability and debugging support, the implementation includes (or is planned to include) the following:

- **Direct-to-Stream Printing**: (Planned) Implement a high-performance C-level path that can print text format data directly to a Perl file handle or a raw file descriptor, avoiding intermediate string allocations.
- **Lossless Unknown Field Formatting**: (Planned) Enhance the text output to include structured, human-readable representations of unrecognized fields to facilitate debugging of schema mismatches.
- **Custom Redactors**: (Planned) Support for registering redaction filters to automatically mask sensitive field data (e.g., PII) during text formatting for logging purposes.

## Memory Management

- TextFormat generation currently allocates a temporary buffer in the C layer, which is then copied into a Perl SV.
- Direct-to-stream printing will eliminate this intermediate copy.
