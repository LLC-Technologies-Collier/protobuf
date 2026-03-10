# LLM Plan Execution Instructions for Perl Protobuf Module

[TOC]

## Goal

You are an AI assistant tasked with implementing a Perl Protocol Buffers module by methodically following the detailed plans provided and adhering to strict development practices.

## Input Files & Context:

*   **Main Plan:** `@perl/doc/ProtobufPlan.md` (Links to C and Perl layer plans)
*   **C Layer Plan:** `@perl/doc/ProtobufPlan-C.md`
*   **Perl Layer Plan:** `@perl/doc/ProtobufPlan-Perl.md`
*   **Architecture Docs:** `@perl/doc/architecture/**` (Consult for design details)
*   **Coding Guidelines:** `@perl/doc/guidelines/**` (CRITICAL to follow all, especially `how-to-write-perl-good.md`)
*   **Development Cycle:** `@perl/doc/internal/development_cycle.md`
*   **XS Learnings:** `@perl/doc/internal/xs_learnings.md`
*   **Existing Code:** `@perl/xs/**`, `@perl/lib/**`, `@perl/t/**`
*   **UPB Source:** `@upb/**` (For UPB API reference)
*   **Python Inspiration:** `@python/**` (For structural and API inspiration)
*   **Test Differences:** `@tmp/python-perl-test-differences.md` (Context for test coverage goals)

## Core Instructions:

1.  **Sequential Execution:** Execute Milestones and tasks in order: first, all of `ProtobufPlan-C.md`, then all of `ProtobufPlan-Perl.md`.
2.  **TDD Workflow:** For each item in the plans:
    *   **Step 1: Create Test File:** If the test file doesn't exist, create it (e.g., `touch perl/t/c/component.c` or `touch perl/t/01-component.t`). Report completion.
    *   **Step 2: Write Tests:** Implement comprehensive tests for the functionality described in the plan item. Aim for full coverage of the C functions or Perl API. Report completion.
    *   **Step 3: Run Tests (Expect Failure):** Execute the tests. Report the *exact* command used and confirm they fail as expected.
    *   **Step 4: Implement Code:** Write the C or Perl/XS code to satisfy the tests.
    *   **Step 5: Run Tests (Expect Success):** Run the tests again using the *exact* command. Report success or failure. **Do not proceed if tests fail.**
        *   **Debugging C Tests:** If C tests fail, especially with segfaults, re-run the *specific failing test target* with verbose output. Instrument the C test code and any involved XS functions with `fprintf(stderr, ...)` to trace execution and variable values. Consider using ASan as described in `perl/doc/architecture/testing/01-xs-testing.md`.
    *   **Step 6: Leak Tests (Perl Layer):** For Perl modules, add leak tests using `Test::LeakTrace` as indicated in the plan. Ensure these pass.
    *   **Step 7: Refactor:** Optional: Refactor for clarity, efficiency, or to better adhere to conventions, ensuring tests still pass.
    *   **Step 8: Report Task Complete:** Once all tests for the specific checklist item pass, report the checklist item as complete.
3.  **Strict Conventions:** All code MUST adhere to `@perl/doc/guidelines/**`.
4.  **C-Layer Commands:**
    *   Change directory: `cd perl`
    *   Build library: `make -j$(nproc) libprotobuf_common.a > /dev/null 2>&1`
    *   Build a C test: `make -j$(nproc) t/c/test_name > /dev/null 2>&1`
    *   Run a C test: `t/c/test_name`
    *   Force rebuild C test: `rm -f t/c/test_name && make -j$(nproc) t/c/test_name > /dev/null 2>&1`
5.  **Perl-Layer Commands:**
    *   Change directory: `cd perl`
    *   Build: `perl Makefile.PL > /dev/null && make -j$(nproc) > /dev/null 2>&1`
    *   Run a Perl test: `prove -bv t/01-some_test.t`
    *   Run all Perl tests: `make -j$(nproc) test` (which uses `prove -b -j$(nproc) t/*.t xt/*.t` as defined in Makefile.PL)
6.  **Makefile Updates:** When adding/removing `*.c` files in `xs/` or `*.xs` files in `lib/`, update `perl/Makefile.PL` accordingly. Re-run `cd perl && perl Makefile.PL > /dev/null` after changes.
7.  **Status Updates:** After completing each checklist item in the plan files (e.g., "[ ] Create test file t/c/protobuf_arena.c"), clearly state that the item is complete. I (the human) will update the checkboxes in the plan files.
8.  **Honest Reporting:** If tests fail, report the failure and the output. If you need to deviate from the plan or make significant changes to tests after starting implementation, state this and await confirmation.

## Initial Task:

Begin with Milestone 1 in `@perl/doc/ProtobufPlan-C.md`.

Let's start the audit and implementation. First, report the status of the first item: `[ ] Setup build system (MakeMaker) for Perl & XS.` by inspecting `perl/Makefile.PL`.

As you proceed, detect whether a milestone or some portion of the
stages of a milestone have been completed by checking for the
existence and success of the tests therefor ; do not redo work which
has already been performed and tested.
