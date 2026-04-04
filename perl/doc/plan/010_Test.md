# Milestone 1.1: C-Level Test Framework Enhancements

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Implement basic C-level test macros (`plan`, `ok`, `is`, `is_string`, etc.).
*   [x] Implement `TODO` functionality for C tests (Schwern-style).
*   [x] Update `perl/t/c/upb-perl-test.h` and `perl/t/c/upb-perl-test.c` with these enhancements.
*   [x] Backfill existing C tests with `TODO` stubs for missing functionality.
*   [x] Add SKIP functionality for C tests.
*   [x] Implement indented subtest support in the C harness (TAP 13 compliant).
*   [x] Implement LEAK_CHECK(arena, block, name) block (C-layer).
*   [x] Implement STRESS_THREADS(n, func, arg) macro for standardized C-level concurrency verification.
*   [x] Add failure probability configuration to StatsAlloc.
*   [x] Implement chaos recovery tests for arena blocks.
*   [x] Verify branch coverage for specific SIMD kernels. (Verified in `t/416_utils_names.t`).
*   [x] Add assembly-level instrumentation for AVX2 paths. (Implemented in `utils.c` with `AVX2_INSTRUMENT`).
*   [x] Implement Binary-Diff Serialization verification for canonical output stability. (Implemented in `PerlUpb_VerifyBinaryDiff`).

## Engineering Excellence (Reach for More)
    *   [ ] Implement Test::Deep style data structure comparison macros. (Difficulty: 4/10)
    *   **Add Property-Based Testing hooks to the C test harness:**
        *   [ ] Design API for property-based test macros in C. (Difficulty: 3/10)
        *   [ ] Implement basic generator for integers. (Difficulty: 2/10)
        *   [ ] Implement shrinker for integers. (Difficulty: 2/10)
        *   [ ] Integrate with C test harness. (Difficulty: 2/10)
    *   **Auto-generate TODOs from C benchmark results:**
        *   [x] Design format for benchmark output to be parsable. (Difficulty: 2/10) - See `perl/doc/architecture/080-C-Benchmark-Format.md`
        *   [ ] Implement parser for benchmark results. (Difficulty: 3/10)
        *   [ ] Implement logic to compare results against thresholds. (Difficulty: 3/10)
        *   [ ] Implement generator for TODO stubs in test files. (Difficulty: 3/10)
    *   [ ] Fuzz Test Harness Macros to ensure robustness. (Difficulty: 5/10)
    *   [ ] Add TAP Protocol Conformance Tests for the C harness. (Difficulty: 4/10)
    *   [ ] Create Unit Tests for each C Test Macro. (Difficulty: 3/10)
    *   [ ] Implement Hyper-fast Procedural Test-Data Generation. (Difficulty: 7/10)
    *   [ ] Implement C-level Mocking Framework. (Difficulty: 6/10)
    *   [ ] Implement Automatic Test Case Generation from Schema. (Difficulty: 8/10)

## Implemented Macros
*   `plan(n)`
*   `ok(val, name)`
*   `is(got, expected, name)`
*   `isnt(got, expected, name)`
*   `is_u(got, expected, name)`
*   `isnt_u(got, expected, name)`
*   `is_string(got, expected, name)`
*   `is_blob(got, expected, len, name)`
*   `is_string_view(got, expected, len, name)`
*   `like(str, pattern, name)`
*   `like_n(str, len, pattern, name)`
*   `subtest(name, block)`
*   `LEAK_CHECK(arena, block, name)`
*   `STRESS_THREADS(n, func, arg)`
*   `cdiag(fmt, ...)`
*   `SKIP(reason, count)`

, block, name)`
*   `STRESS_THREADS(n, func, arg)`
*   `cdiag(fmt, ...)`
*   `SKIP(reason, count)`

