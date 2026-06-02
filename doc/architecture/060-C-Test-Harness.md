# C Test Harness Architecture

[TOC]

## Overview

The C-level test harness (`perl/t/c/upb-perl-test.h` and `.c`) provides macros and functions to facilitate testing the C/XS components of the `Protobuf` module. It aims to produce TAP 13 compliant output.

## Implemented Macros

See `perl/doc/plan/010_Test.md` for a list of implemented macros.

## Engineering Excellence Goals

-   **Test::Deep Style Comparisons:** Implement macros for deep comparison of C data structures, similar to Perl's `Test::Deep`.
-   **Property-Based Testing:** Integrate hooks for property-based testing, allowing for random data generation and shrinking to find minimal failing cases.
-   **Benchmark Driven TODOs:** Automatically analyze benchmark results and generate `TODO` stubs in test files for areas needing performance-related tests.
-   **Harness Fuzzing:** Fuzz the inputs to the test macros themselves to ensure the harness is robust.
-   **TAP Protocol Conformance:** rigorously test the harness output against the TAP 13 specification.
-   **Macro Unit Tests:** Develop a dedicated test suite for the macros defined in `upb-perl-test.h`.
