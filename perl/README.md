# Protobuf - High-performance Perl Protocol Buffers using upb

Protobuf is a high-performance Protocol Buffers implementation for Perl, built
upon the efficient `upb` C library. It provides a memory-safe, thread-isolated
environment with advanced features like thread-local arena caching, SIMD-accelerated
conversions, and zero-copy IPC support.

## Features

- **Extreme Performance:** Utilizes the `upb` C library and VPP-inspired patterns.
- **Memory Safety:** Implements 16-byte canary guards for all arena-allocated blocks.
- **Thread Isolation:** Interpreter-local state via a per-interpreter registry.
- **Advanced Containers:** Direct-to-hash projection for maps and descriptors.
- **Well-Known Types:** Full support for Any, Duration, Timestamp, Struct, etc.
- **JSON & Text Format:** High-speed serialization and parsing.

## Prerequisites

This Perl module links against components built by Bazel. Ensure that you have Bazel installed and have built the necessary dependencies from the root of the repository. Typically, this involves running a command similar to:

```bash
# From the repository root directory (one level up)
bazel clean --expunge && bazel build //src/google/protobuf:descriptor_proto //src/google/protobuf:descriptor_upb_c_proto
```

## Installation

```bash
perl Makefile.PL
make
make test
sudo make install
```

## Usage

```perl
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
$pool->add_serialized_file_descriptor_set($data);

my $msg = My::Generated::Message->new();
$msg->set_value(42);
my $wire = $msg->serialize();
```

## License

This software is licensed under the Apache License 2.0.
