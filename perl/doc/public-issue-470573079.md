# Public Issue Draft for Feature Request

**Component:** Public Trackers > Cloud Platform > Data Analytics > Cloud Dataproc

**Title:** Feature Request: Enhance Perl & Raku Support in GCP - Protocol Buffers Libraries

**Please describe your requested enhancement. Good feature requests will solve common problems or enable new use cases.**

This request is to enhance support for the Perl and Raku programming languages within the Google Cloud Platform. The primary goal is to enable developers using these languages to more easily and efficiently interact with GCP services and other systems that leverage Protocol Buffers for data exchange. This involves creating and maintaining high-quality Protocol Buffers libraries for both languages.

**What you would like to accomplish:**

1.  Provide Perl developers with a robust, performant, and idiomatic Protocol Buffers library.
2.  Provide Raku developers with a similar high-quality Protocol Buffers library.
3.  Lower the barrier for integrating applications written in Perl and Raku with GCP services that use gRPC or Protocol Buffers.
4.  Enable new use cases by allowing the large existing ecosystems of Perl and Raku tools and applications to connect to the GCP environment.
5.  Recognize and support The Perl and Raku Foundational languages as valuable parts of the broader software development landscape.

**How this might work:**

1.  **Phase 1 (Perl):**
    *   Develop a new Perl module (e.g., `Protobuf::UPB` or similar) that wraps the core `upb` C library (https://github.com/protocolbuffers/upb).
    *   Utilize Perl's XS system for the C interface to maximize performance.
    *   Provide a comprehensive API for message definition, serialization, deserialization, and reflection, mirroring the features available in other first-class language implementations.
    *   Include thorough testing and documentation.

2.  **Phase 2 (Raku):**
    *   Develop a Raku module with similar goals, also wrapping the `upb` C library.
    *   Utilize Raku's NativeCall capabilities or other appropriate FFI mechanisms for interfacing with C.
    *   Ensure the API is idiomatic and natural for Raku developers.

**If applicable, reasons why alternative solutions are not sufficient:**

*   Existing pure Perl Protocol Buffers implementations may not offer the same level of performance as a C-backed library, especially for large datasets or high-throughput scenarios.
*   Lack of a well-maintained and comprehensive library discourages the use of Perl and Raku in cloud-native applications interacting with modern RPC systems like gRPC.
*   A Google-supported or community-supported upb-based module would provide a clear and reliable path forward.

**Other information (workarounds you have tried, documentation consulted, etc):**

*   This development is being prototyped, and progress can be tracked via related commits and issues.
*   The `upb` library is the current C/C++ souce of truth for protobuf in C, and is used by other language implementations (e.g., Ruby, PHP, Python).
*   Consulting existing `upb` wrappers in other languages for best practices.

This initiative aims to make Google Cloud more accessible and attractive to the established and active Perl and Raku communities.
