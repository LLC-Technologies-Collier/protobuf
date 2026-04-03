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
    *   [ ] Add Property-Based Testing hooks to the C test harness. (Difficulty: 6/10)
    *   [ ] Auto-generate TODOs from C benchmark results. (Difficulty: 7/10)

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

