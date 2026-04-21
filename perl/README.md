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

This Perl module requires Bazel to build the `protoc-gen-perl-pb` plugin. Ensure you have Bazel installed. The necessary C++ Protobuf libraries used by the plugin are typically fetched and built by Bazel as part of the main project's workspace setup.

Additionally, you'll need a C++ compiler (like g++), Make, and the Perl development headers.

To install Perl module dependencies, you can use `cpanm`:

```bash
# Install cpanminus and Carton
cpanm App::cpanminus Carton

# From the perl/ directory
carton install
```
This will install dependencies listed in the `cpanfile`.

## Building the Plugin

The `protoc-gen-perl-pb` plugin is built automatically via Bazel when you run `make` in the `perl/` directory, as configured in `Makefile.PL`.

## Installation

To build and test the Perl module:

```bash
# From the perl/ directory
perl Makefile.PL
make -j$(nproc)
make test
sudo make install
```

To run a full clean test cycle, including rebuilding the plugin:

```bash
# From the perl/ directory
# Clean up previous Bazel run for the plugin
bazel --output_base=/usr/local/google/home/cjac/.gemini/tmp/protobuf/bazel_output_base clean --expunge
# Clean up MakeMaker build
make clean
# Regenerate Makefile and build everything including the plugin, then test
perl Makefile.PL && make -j$(nproc) && make test
```
**Note:** We use a specific Bazel `output_base` to ensure cache consistency between different shells. If you run Bazel commands manually in this workspace, you should use the same output base:
`alias bazel='bazel --output_base=/usr/local/google/home/cjac/.gemini/tmp/protobuf/bazel_output_base'`


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
