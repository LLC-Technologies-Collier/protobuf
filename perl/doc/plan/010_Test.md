# Milestone 1.1: C-Level Test Framework Enhancements

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Implement basic C-level test macros (`plan`, `ok`, `is`, `is_string`, etc.).
*   [x] Implement `TODO` functionality for C tests (Schwern-style).
*   [x] Update `perl/t/c/upb-perl-test.h` and `perl/t/c/upb-perl-test.c` with these enhancements.
*   [x] Backfill existing C tests with `TODO` stubs for missing functionality.
*   [x] Add `SKIP` functionality stub for C tests.
*   [x] Implement indented subtest support in the C harness (TAP 13 compliant).
*   [x] Implement `LEAK_CHECK { ... }` block stub (C-layer).
*   [ ] TODO: Add `STRESS_THREADS(n, func)` macro for standardized C-level concurrency verification.

## Implemented Macros
*   `plan(n)`
*   `ok(val, name)`
*   `is(got, expected, name)`
*   `is_u(got, expected, name)`
*   `is_string(got, expected, name)`
*   `is_blob(got, expected, len, name)`
*   `is_string_view(got, expected, len, name)`
*   `like(str, pattern, name)`
*   `like_n(str, len, pattern, name)`
*   `subtest(name, block)`
*   `LEAK_CHECK(block)`
*   `cdiag(fmt, ...)`

