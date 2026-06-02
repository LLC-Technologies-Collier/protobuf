```markdown
# Development Project Execution

You are an AI assistant helping with a software development project. Your goal is to execute the tasks outlined in the provided plan, adhering strictly to the project's conventions and methodological guidelines.

**Input Files:**

1.  **Plan:** `@tmp/plan-for-refactoring-large-modules-into-smaller-modules-and-testing.md`
2.  **Conventions:** `@cjproc/conventions.md`

**Core Instructions:**

1.  **Follow the Plan:** Execute the steps outlined in the plan document in order.
2.  **Adhere to Conventions:** All code modifications and additions *must* follow the rules in the conventions document.
3.  **Refactoring:** When refactoring, follow the Test-Driven Development (TDD) approach outlined in `@cjproc/share/prompts/how-to-write-perl-good.md` (Section 21).
4.  **Adhere to Methodological Guidelines:** The "Methodological Guidelines" section in the plan document is paramount. Pay close attention to:
    *   Strict test output checking.
    *   Running tests twice to confirm success.
    *   Incremental changes and testing.
    *   Proper mocking techniques.
    *   Build order (`perl Makefile.PL && make`).
    *   Running `t/01_critic.t` after each module's refactoring is complete.
    *   **Acceptance Testing:** Remember that mocking within the test file doesn't affect scripts run via `IPC::Run`. Test side effects or use logging for verification.
4.  **TODO List:** Maintain the TODO list accurately, only marking items complete when all associated checks and tests have passed *twice* and without any console noise.
5.  **Communication:** Report on progress, and importantly, any failures or ambiguities encountered. Do not proceed if tests fail; debug and fix the issue first.
6.  **Proactive Critic:** Strive to write code that *inherently* meets Perl::Critic standards as defined in `@cjproc/conventions.md` and `@cjproc/share/prompts/how-to-write-perl-good.md`. Addressing violations *as you write* is far more efficient than fixing them later. **Crucially, all regexes MUST use the `/x` modifier.** Remember to escape literal spaces and `#` characters within `/x` blocks.
7.  **Handling Persistent Perl::Critic Violations:** If `t/01_critic.t` reports violations, attempt to fix them based on the error message and the guidelines in `how-to-write-perl-good.md`. If a specific policy violation persists after two fix attempts, consult the policy's PDF in `pbp/policy_docs/`. The PDF filename is the policy name with `::` replaced by `_` (e.g., `Modules_RequireUseStrict.pdf`).

**Execution Flow:**

*   For each major step in the "Detailed Steps" section of the plan:
\
    *   Perform the refactoring and test creation as described.
    *   Follow the "Test-Driven Development for Sub-Modules" and "Methodological Guidelines".
    *   Ensure all relevant unit tests pass by running `prove -bv t/your_test_file.t` from the `cjproc` directory (run twice).
    *   Run `prove -b t/01_critic.t` from the `cjproc` directory to check for new violations.
    *   Finally, run `make test` from the `cjproc` directory (run twice) to ensure the full suite is green.
*   Update the TODO list accordingly.

Let's start executing the plan from the current state.
```