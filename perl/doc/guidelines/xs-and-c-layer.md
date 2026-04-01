# XS and C-Layer Development Guidelines

This document outlines the specific conventions and testing patterns for the XS and C-layer implementation of this project.

## Directory Structure
- `perl/lib/`: Contains `.pm` modules and their corresponding `.xs` bindings.
- `perl/xs/`: Contains pure C logic (source and headers) that are linked into the common shared library or used by the XS bindings.
- `perl/t/c/`: Contains C-level unit tests.
- `perl/xt/`: Contains extended integration tests (e.g., concurrency testing with Coro/Mojo).

## Planning Documents
- All planning documents in `perl/doc/plan/` MUST use paths relative to the project root (e.g., `perl/xs/descriptor.c`) to avoid ambiguity.
- Milestones are not considered complete until literal compliance (existence of files, passing tests, specified functionality) is verified on disk.

## Testing Conventions
### C-Level Unit Tests
- Use the test harness provided in `perl/t/c/upb-perl-test.h`.
- Follow the TAP (Test Anything Protocol) format.
- **TODO Blocks:** Use the `TODO("reason") { ... }` macro to track planned coverage that is not yet implemented. This allows the test suite to pass while explicitly flagging missing coverage.
- **XS-Based C Runner:** For C logic that requires a Perl interpreter context (`aTHX`), use the XS runner defined in `perl/t/c/Test.xs`.

### Concurrency Testing
- Concurrency safety MUST be verified using `libcoro` (in `t/c/integration/`) and Perl-level event loops like `Mojo::IOLoop` (in `xt/mojo/`).
- Integration tests should stress re-entrancy and thread-local state management in the C layer.

## Python Parity
- The primary goal of this implementation is full feature parity with the Python upb-based implementation.
- Alignment with Python's behavior for `AddSerializedFile`, `DescriptorPool` resolution, and `SymbolDatabase` is mandatory.
- C-layer utilities should be designed to support the eventual high-level Python-like APIs in Perl.

## Memory Safety and Debugging
- All tests MUST pass under **AddressSanitizer (ASan)**.
- Use `detect_leaks=1` in `ASAN_OPTIONS` during local development to catch memory leaks early.
- Weak references in the object cache MUST be rigorously validated to prevent stale pointer usage.
