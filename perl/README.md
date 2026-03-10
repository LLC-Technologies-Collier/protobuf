# Protobuf - Perl Protocol Buffers

This module provides a Perl interface to Google's Protocol Buffers, using the
[UPB](https://github.com/protocolbuffers/protobuf/tree/main/upb) library for
its core implementation.

## Prerequisites

To build and install this module, you will need:

1.  **The UPB development library:**
    *   On Debian/Ubuntu: `sudo apt-get install libupb-dev`
    *   Other systems: Install the appropriate package for UPB headers and libraries.

2.  **The Protocol Buffers Compiler (`protoc`):**
    *   On Debian/Ubuntu: `sudo apt-get install protobuf-compiler`
    *   Other systems: Download from [protobuf releases](https://github.com/protocolbuffers/protobuf/releases).

3.  **Standard Perl build tools:** `make`, a C compiler (like `gcc`).

## Building

```bash
perl Makefile.PL
make
make test
sudo make install
```

## Status

This module is under development.
