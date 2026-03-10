# How to Prepare an LLM to Succeed at Software Engineering Tasks

Based on my experience during our recent refactoring session, here are key strategies to guide an LLM to perform software engineering tasks effectively, mimicking a mature engineering approach:

1.  **Provide Clear Goals and Context:**
    *   **Overarching Objective:** Clearly state the main goal (e.g., "Refactor the Salesforce Case fetching pipeline to align with the Buganizer pipeline's architecture").
    *   **Key Constraints & Requirements:** Specify essential constraints (e.g., "Must use `csdata.case_detail` due to permissions," "Maintain existing schema compatibility").
    *   **Existing Patterns:** Explicitly point out existing modules or patterns to emulate (e.g., "Model the new test on `@cjproc/t/21-fetcher-salesforcecasebydate.t`").

2.  **Iterative Development & Testing:**
    *   **Small, Testable Changes:** Encourage the LLM to make small, incremental changes rather than large, complex ones. This makes debugging much easier.
    *   **Frequent Testing:** Instruct the LLM to run specific, relevant tests after each significant code modification. Provide the exact test commands.
    *   **Full Test Suite Runs:** After a set of related changes, run the broader test suite to catch unintended side effects.
    *   **Real-World Validation:** When tests pass, validate with real-world script execution (like `fetch-case-details.pl`) to catch issues tests might miss (e.g., permissions, data format mismatches from live services).

3.  **Debugging Guidance:**
    *   **Interpret Errors:** Guide the LLM to analyze error messages and stack traces to understand the root cause.
    *   **Isolate Problems:** When errors occur, suggest techniques to isolate the issue, such as:
        *   Examining input/output files (`tail`, `cat -E`, `od -c`).
        *   Adding temporary debug prints/logs in the code.
        *   Running smaller sections of code or tests.
    *   **Don't Assume:** Remind the LLM not to assume the cause of an error. For example, an EOF error in CSV parsing might not be a truncated file but an unescaped quote in the *previous* line.
    *   **Consider the Whole Pipeline:** An error in one module (e.g., Writer) might stem from bad data produced by an earlier module (e.g., Parser).

4.  **Enforce Consistency:**
    *   **Naming Conventions:** Explicitly call out the need for consistent naming of methods, variables, and modules (e.g., `fetch_by_id`, `fetch_by_date`).
    *   **Code Style:** Remind the LLM to mimic the style of the surrounding code.
    *   **Architectural Patterns:** Reinforce the desired architectural patterns (e.g., F1Runner -> Parser -> Processor -> Writer).

5.  **Handling Tools and Environment:**
    *   **Tool Usage:** Be specific about which tools to use for which tasks (e.g., `replace` for simple changes, `read_file` for context, `run_shell_command` for execution).
    *   **File Paths:** Use full paths or paths relative to the project root, and be clear about the context directory.
    *   **Cache Management:** Explain when and how to clear caches (`--force-refresh`) when underlying data or queries change.

6.  **Learning and Correction:**
    *   **Journaling:** Encourage the LLM to document learnings, especially from mistakes or debugging sessions (as we did with `learnings-journal.md`).
    *   **Correcting the LLM:** When the LLM makes a mistake, provide clear, specific feedback on what was wrong and why, and how to correct it. Refer back to previous instructions or examples.
    *   **Positive Reinforcement:** Acknowledge when the LLM does things correctly ("Yes, that's much better!").

By providing this kind of structured guidance and feedback, you can help an LLM develop a more robust and effective approach to software engineering tasks, much like mentoring a human engineer.
