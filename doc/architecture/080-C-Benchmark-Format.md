# C Benchmark Output Format

To enable automatic analysis and TODO generation, C benchmarks run via `make bench` should output results in a machine-readable format.

Each benchmark run should output a single line of JSON to STDOUT.

## Fields:

-   `suite`: (String) The name of the benchmark suite.
-   `name`: (String) The specific name of the benchmark test.
-   `iterations`: (Integer) The number of iterations performed.
-   `time_ns`: (Integer) Total time taken in nanoseconds.
-   `bytes_per_sec`: (Float, Optional) Throughput in bytes per second, if applicable.
-   `ops_per_sec`: (Float, Optional) Throughput in operations per second, if applicable.
-   `error`: (String, Optional) Any error message encountered during the benchmark.

## Example:

```json
{"suite": "ArenaBench", "name": "AllocFree", "iterations": 1000000, "time_ns": 50000000, "ops_per_sec": 20000000}
{"suite": "MessageBench", "name": "ParseSmall", "iterations": 10000, "time_ns": 120000000, "bytes_per_sec": 85000000}
```

This format can be easily parsed by scripts to compare against baselines or thresholds.