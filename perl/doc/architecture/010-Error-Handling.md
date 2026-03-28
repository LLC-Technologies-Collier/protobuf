# Architecture: Error Handling

[TOC]

## C Layer (XS)

1.  **upb Errors:** Errors originating from the `upb` library functions should be checked meticulously.
2.  **Propagation to Perl:** C functions called from Perl (XS) MUST have a clear way to signal errors back to the Perl layer. This will primarily be done by returning `NULL` or a special sentinel value for functions expected to return pointers, and a dedicated boolean/integer status for others.
3.  **Perl Exceptions:** Upon detecting an error from `upb` or internal C logic, the XS wrapper code should construct a descriptive error message and use `croak()` to throw a Perl exception.
4.  **Error Messages:** Error messages should be informative, including the function that failed and the reason, if available from `upb`.

## Perl Layer

1.  **Exception Based:** The Perl API will use exceptions (die/croak) to signal error conditions.
2.  **Specific Error Classes:** (Consideration for the future) We may introduce specific error classes inheriting from a base `Protobuf::Error` to allow for more granular error handling by users.
