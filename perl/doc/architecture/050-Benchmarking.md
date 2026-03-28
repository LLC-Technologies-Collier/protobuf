# Architecture: Benchmarking

[TOC]

1.  **Framework:** Use `Benchmark::Dumb` for simplicity and low overhead.
2.  **Targets:** Key operations to benchmark:
    *   Message creation.
    *   Field access (set/get for various types).
    *   Serialization (to binary string).
    *   Deserialization (from binary string).
    *   TextFormat and JSON en/decoding.
3.  **Comparisons:** Where feasible, compare against:
    *   `Google::ProtocolBuffers`
    *   `Protobuf::XS`
4.  **Reports:** Benchmarking results should be logged and summarized.
