# Milestone 1.5: Makefile.PL Refactoring with Template Toolkit

[TOC]

## Objective

Refactor `perl/Makefile.PL` to improve readability and maintainability by using the `Template` module (Template Toolkit) to generate complex Makefile sections within `MY::postamble` and other `MY::*` overrides. This will separate Makefile syntax into template strings or files, distinct from the Perl logic that populates them.

## Status Update (2026-04-04)

Revising plan to use Template Toolkit directly within `Makefile.PL`'s `MY::` overrides, without an external helper module. Each step will be small and result in a testable build.

## Refactoring Plan using Template Toolkit

The core idea is to embed Makefile fragments as strings within `Makefile.PL` and process them with `Template`. Later, these can be moved to separate `.mk.tt` files if desired.

**Overall Workflow for Each Migration Step:**

1.  **Identify:** Pinpoint a section of Makefile generation logic within `Makefile.PL` (e.g., in `MY::postamble`).
2.  **Template String:** Create a Perl multi-line string variable containing the Template Toolkit version of the Makefile syntax.
3.  **Render:** In the `MY::*` function, instantiate a `Template` object, prepare a hash of variables, and process the template string to generate the Makefile fragment.
4.  **Integrate:** Replace the old string concatenation logic with the output from the template processing.
5.  **Verify:** Run the full build and test pipeline to ensure no regressions:
    ```bash
    cd perl && 
    perl Makefile.PL && 
    make clean && 
    make -j$(nproc) && 
    make -j$(nproc) test_c && 
    make -j$(nproc) test && 
    make -j$(nproc) bench
    cd ..
    ```

## Tasks

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] REFRESH: Review Template Toolkit and ExtUtils::MakeMaker documentation.

1.  **[x] Initial Setup:**
    *   [x] Add `Template` to `PREREQ_PM` in `perl/Makefile.PL`.
    *   [x] Add `use Template;` at the top of `perl/Makefile.PL`.
    *   [x] VERIFY: Build and Test Pass.

2.  **[x] Template Rendering Helper:**
    *   [x] Create a small helper function `_render_tt( $tt_string, $vars )` within `perl/Makefile.PL` (outside any package). This function will create a new `Template` object configured with `PRE_CHOMP => 1, POST_CHOMP => 1, TRIM => 0` and process the given string.
    *   [x] VERIFY: Build and Test Pass.

3.  **[x] Migrate `COMMON_LIB` Definition:**
    *   [x] Convert the `COMMON_LIB = $common_lib` line in `MY::postamble` to a TT template string.
    *   [x] Use `_render_tt` to render this template.
    *   [x] VERIFY: Build and Test Pass.

4.  **[x] Migrate Common Object File Rules:**
    *   [x] Convert the `foreach my $pair (@$file_pairs)` loop in `MY::postamble` to a TT template string.
    *   [x] Pass `$file_pairs` to `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

5.  **[x] Migrate `libprotobufperl` Link Rule:**
    *   [x] Convert the EOM block for linking `$(COMMON_LIB)` to a TT template string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

6.  **[x] Migrate `upb-perl-test.o` Rule:**
    *   [x] Convert the EOM block for `t/c/upb-perl-test.o` to a TT template string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

7.  **[x] Migrate C Test Rules:**
    *   [x] Convert the C test rules generation loop to a TT template string.
    *   [x] This involves passing `c_test_config`, paths, and flags to the template.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

8.  **[x] Migrate C Benchmark Rules:**
    *   [x] Convert the C benchmark rules generation loop to a TT template string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

9.  **[x] Migrate PHONY, test_descriptor.bin, build_c_tests, build_c_bench:**
    *   [x] Convert the remaining static and conditional blocks in `MY::postamble` into one or more TT template strings.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

10. **[ ] Migrate Test XS Rules (in MY::test):**
    *   This is more complex as it affects `MY::test`. The logic for finding `t/lib/Protobuf/Internal/Test/*.xs` needs to be done *before* `WriteMakefile`.
    *   The generation of the rules themselves can be a template string processed within `MY::postamble` and the resulting list of targets passed to `WriteMakefile`'s `clean => { FILES => ... }` and used in `MY::test` dependencies.
    *   **Sub-Step 10.1:** Calculate Test XS files and object names before `WriteMakefile`.
    *   **Sub-Step 10.2:** Create template string for Test XS compile rules in `MY::postamble`.
    *   **Sub-Step 10.3:** Render template in `MY::postamble` to get rule fragment.
    *   **Sub-Step 10.4:** Update `clean` in `WriteMakefile` with Test XS objects.
    *   **Sub-Step 10.5:** Update `MY::test` to depend on the Test XS .so targets.
    *   [ ] VERIFY: Build and Test Pass after each sub-step.

11. **[x] Optional: Move Templates to Files:**
    *   [x] Create `perl/inc/templates/` directory.
    *   [x] Move each template string into its own `.mk.tt` file (e.g., `common_lib.mk.tt`, `c_test.mk.tt`, etc.).
    *   [x] Update `_render_tt` to accept a file name and configure `Template` with `INCLUDE_PATH => ['inc/templates']`.
    *   [x] VERIFY: Build and Test Pass.

12. **[ ] Documentation:**
    *   [ ] Update `perl/doc/architecture/build_system.md`.
    *   [ ] Mark this document (015_Makefile_Refactor.md) as complete.

## Engineering Excellence (Reach for More)
    *   [ ] Use more conditional logic within templates to reduce Perl-side complexity. (Difficulty: 3/10)
    *   [ ] Add a `VERBOSE` option to `perl Makefile.PL` to inject debug comments into the Makefile. (Difficulty: 4/10)
    *   [ ] Create a test suite to validate generated Makefile syntax using `make -n`. (Difficulty: 6/10)

## Lessons Learned & Pitfalls Encountered

1.  **Template Syntax vs. Make Syntax:**
    *   **Pitfall:** Inside the `.mk.tt` files or template strings, any `$` symbols not part of a Template Toolkit directive or variable will be passed through literally. This is generally desired for Make variables like `$(CC)`. Template Toolkit directives use `[% ... %]`.
    *   **Guidance:** Be clear about which variables are for Template Toolkit (`[% var %]`) and which are for Make (`$(VAR)`).

2.  **Whitespace Control:**
    *   **Pitfall:** Template Toolkit's chomping options (`PRE_CHOMP`, `POST_CHOMP`, `TRIM`) can remove newlines critical for separating rules in the Makefile, leading to syntax errors or multiple target patterns errors.
    *   **Guidance:** Use `PRE_CHOMP => 1, POST_CHOMP => 1, TRIM => 0`. Use explicit `[% "
" %]` in templates, especially after `[% END %]` in loops, to ensure rule separation.

3.  **C Test & Benchmark Linking:**
    *   **Guidance:** The templates must generate link commands for test/benchmark executables that include `-L$(CURDIR) -lprotobufperl`.

4.  **Runtime Library Path:**
    *   **Guidance:** The templates must generate test execution commands prepended with `LD_LIBRARY_PATH=$(CURDIR)`.

5.  **Dependencies:**
    *   **Guidance:** Templates must generate rules where test/benchmark executables depend on `$(COMMON_LIB)`.

By separating the Makefile syntax into templates, we isolate the concerns. The Perl code in `Makefile.PL` becomes responsible for logic and data preparation, while the templates handle the Makefile structure. This should significantly reduce the risk of quoting and interpolation errors.
