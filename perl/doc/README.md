# Perl Protobuf Documentation Index

Welcome to the technical documentation for the high-performance Perl Protocol
Buffers runtime. This directory contains comprehensive guides covering the
design, implementation, and execution of the `upb`-based Perl XS extension.

--------------------------------------------------------------------------------

## 📂 Documentation Map

### 1. 🚀 [Implementation Plan & Chronology](IMPLEMENTATION_PLAN.md)

*   **Purpose:** The authoritative history and execution roadmap of the project.
*   **Key Contents:** Detailed weekly logs (Weeks 13–23), milestone breakdowns,
    technical hurdles faced (and how they were resolved), performance tuning
    phases, and CPAN release preparation.
*   **Read this if:** You want to understand the chronological progress,
    completed milestones, or the history of engineering decisions.

### 2. 🏛️ [High-Level Architecture Overview](architecture/OVERVIEW.md)

*   *Formerly `ARCHITECTURE.md`*
*   **Purpose:** A high-level overview of the entire system architecture,
    directory structure, and development workflows.
*   **Key Contents:** Design philosophy, directory layout, dual-build system
    (`Makefile.PL` vs. Bazel/Blaze), testing strategy, benchmarking harness, and
    the JIT (Just-In-Time) vendoring architecture.
*   **Read this if:** You are new to the codebase and want to understand how the
    pieces fit together, how to run tests, or how the build systems operate.

### 3. 🧠 [XS Core Subsystem Deep Dive](architecture/XS_CORE_DEEP_DIVE.md)

*   *Formerly `CORE_ARCHITECTURE.md`*
*   **Purpose:** A low-level technical specification of the C/XS runtime
    subsystems.
*   **Key Contents:**
    *   **Memory Management:** Arena allocation, memory canaries, and
        thread-local arena caching (VPP pattern).
    *   **Object Caching:** Striped-mutex pointer-to-SV mapping, LRU eviction,
        and ring-buffered audit logging.
    *   **Type Conversion:** Bi-directional marshalling between Perl SVs and
        `upb_MessageValue`, Math::BigInt promotion, and SIMD-accelerated UTF-8
        validation.
    *   **Shared Memory IPC:** Zero-copy `tmpfs`-backed message sharing and
        SELinux mandatory access controls.
    *   **Error Propagation:** `upb_Status` translation and context-aware
        `croak` exceptions.
*   **Read this if:** You are modifying the C or XS code under `xs/` and need to
    understand the memory lifecycles, concurrency guards, or type-mapping
    nuances.

### 4. 🎨 [Perl API Design & Conventions](architecture/API_DESIGN.md)

*   **Purpose:** Guidelines and specifications for the user-facing Perl API.
*   **Key Contents:** Moo object system conventions, naming standards
    (snake_case vs. CamelCase), dynamic method injection via `ClassGenerator`
    (avoiding `AUTOLOAD`), and tied array/hash container behaviors.
*   **Read this if:** You are modifying the Perl modules under `lib/` or want to
    understand how the public API is presented to end-users.

--------------------------------------------------------------------------------

## 🛠️ Documentation Maintenance

All architecture and design documents should be updated when:

1.  New XS subsystems or core utilities are introduced.
2.  Significant changes are made to the memory management or caching models.
3.  The public Perl API undergoes breaking changes or major enhancements.
