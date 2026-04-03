# Milestone 1.5: Makefile.PL Refactoring

[TOC]

## Objective

Refactor the `perl/Makefile.PL` to improve readability and maintainability by moving complex rule generation logic out of `MY::postamble` and into a helper module or separate fragment files.

## Tasks

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Create `inc/MyMakeHelper.pm` directory and module structure. (Difficulty: 1/10)
*   [ ] Move C test rule generation logic from `MY::postamble` to `inc/MyMakeHelper.pm`. (Difficulty: 3/10)
*   [ ] Move C benchmark rule generation logic to `inc/MyMakeHelper.pm`. (Difficulty: 3/10)
*   [ ] Move rule generation for `libprotobufperl.$(DLEXT)` to `inc/MyMakeHelper.pm`. (Difficulty: 2/10)
*   [ ] Add logic to `inc/MyMakeHelper.pm` to build test-only XS modules in `t/lib`. (Difficulty: 4/10)
*   [ ] Update `Makefile.PL` to `use inc::MyMakeHelper` and call its functions. (Difficulty: 2/10)
*   [ ] Verify that `perl Makefile.PL && make && make test` works identically after refactoring. (Difficulty: 2/10)
*   [ ] Review and update relevant architecture/guideline documents.
