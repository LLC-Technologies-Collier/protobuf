# Architecture: Testing

[TOC]

## C-Level Tests

1.  **Harness:** Custom C test harness in `t/c/upb-perl-test.h`.
2.  **Concurrency:** Use `libcoro` to stress test for re-entrancy and thread-safety issues in the C layer.

## Perl-Level Tests

1.  **Test::More:** Standard testing with `Test::More`.
2.  **Test::LeakTrace:** Mandated for all Perl modules wrapping C components to detect memory leaks.
3.  **Test::Exception:** For testing exception-based error handling.
4.  **Devel::Cover:** Aim for >= 90% statement, branch, and condition coverage for all Perl modules. Coverage reports should be generated during the test process.
