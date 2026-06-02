# Release and Lifecycle Management

_Status: Planned_

This document outlines the strategy for packaging, distributing, and maintaining the Protocol Buffers Perl implementation.

## CPAN Distribution

The project follows standard CPAN packaging conventions using `ExtUtils::MakeMaker`.

- **XS Integration**: The distribution includes a shared C library (`libprotobuf_common.so`) and multiple XS modules.
- **Dependencies**: The primary dependency is the `upb` C library, which is bundled or linked depending on the environment.

## Advanced Release Goals

To achieve world-class software distribution and maintainability, the project includes (or is planned to include) the following:

- **Fully Automated CI/CD**: (Planned) Implement a GitHub Actions (or internal) pipeline that automatically runs the full test suite under AddressSanitizer (ASan) and ThreadSanitizer (TSan) for every commit and tag.
- **Embedded Performance Benchmarks**: (Planned) Include a benchmarking suite in the distribution that users can execute to verify library performance on their specific hardware and Perl version.
- **Searchable Architecture Documentation**: (Planned) Provide high-quality, searchable HTML documentation that integrates with MetaCPAN and provides deep insights into the C-layer and XS internals.
- **Binary Distribution (Alien::upb)**: (Planned) Utilize the `Alien` namespace to provide pre-compiled versions of the `upb` library for common platforms, simplifying installation for users without a C compiler.

## Versioning

- The project uses Semantic Versioning (SemVer).
- Version alignment with the official Protocol Buffers release cycle will be maintained where possible.
