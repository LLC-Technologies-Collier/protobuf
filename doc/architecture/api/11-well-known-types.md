# Well-Known Types (WKT)

_Status: Fully Implemented_

The Protocol Buffers Well-Known Types (WKT) provide a standard set of messages for common patterns such as timestamps, durations, and dynamic data (`Any`, `Struct`).

## Implementation

### Dynamic and Wrapper Types
- **`google::protobuf::Any`**: Implements `pack($msg)` and `unpack()` methods. Unpacking uses the `DescriptorPool` to reify the serialized blob.
- **`google::protobuf::Struct`**: Provides a Perl-to-Message bridge (`from_perl`, `to_perl`) to convert between native Perl nested data structures (HashRefs/ArrayRefs) and the `Struct` / `ListValue` protocol types.
- **Wrappers**: Types like `StringValue`, `Int32Value`, etc., are handled by the standard class generation and provide a nullable alternative to scalar fields.

### Temporal Types
- **`google::protobuf::Timestamp`**: Supports conversion to/from POSIX epoch and `Time::Piece` objects.
- **`google::protobuf::Duration`**: Supports conversion to/from floating-point seconds.

## Advanced Performance Goals

To achieve world-class integration for common data patterns, the implementation includes (or is planned to include) the following:

- **Direct Any Unpacking (C-Layer)**: (Planned) Implement a high-performance integration that can unpack an `Any` message directly into a C structure without intermediate Perl object creation, using a cached `upb_DefPool`.
- **Vectorized Temporal Conversions**: (Planned) Implement SSE4.1/AVX2 optimized conversion logic for `Timestamp` and `Duration` types to support high-throughput log processing and event analysis.
- **WKT-Specific Memory Profiling**: (Planned) Provide detailed memory overhead reports for complex `Struct` and `ListValue` objects, aiding in the identification of memory bloat in dynamic protocols.

## Memory Management

- Well-Known Types are standard `Protobuf::Message` objects and follow the same arena-based lifecycle rules.
- `Any->unpack()` creates a new message instance that shares the parent `Any` object's arena unless otherwise specified, ensuring efficient data migration.
