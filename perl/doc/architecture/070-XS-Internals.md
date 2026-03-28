# Architecture: XS Internals & C API

[TOC]

## Purpose of `libprotobuf_common.a`

`libprotobuf_common.a` is intended to be a static library containing custom C helper functions specifically for this Perl-UPB binding. This library is SEPARATE from the core `upb` libraries. It will include functions that facilitate:

*   Marshalling data between Perl SVs and upb types.
*   Common XS error handling routines.
*   Utility functions not provided by `upb` but needed by the XS layer.

This library will be built by the MakeMaker process as a prerequisite for building the XS module.

## C API Naming Conventions for XS

*   Functions intended to be called from XS should be prefixed with `perl_upb_`.
*   Helper functions internal to the C code (not directly called by XS macros) should be declared `static` and follow standard C naming conventions.
*   Clearly document parameters and return values in header files.

## Typemap Strategy

*   **Location:** Typemap entries will be placed in the `perl/typemap` file.
*   **Organization:** Group typemaps by the C type they handle.
*   **Naming:** Use standard typemap conventions.
*   **Usage:** Utilize typemaps to automate the conversion between Perl SVs and C types like `upb_Arena *`, `upb_Message *`, `upb_MiniTable *`, etc., and various descriptor types.

## Data Passing Between Perl and C

*   **Input to C:** Functions taking string data from Perl should expect to receive a `char *` and potentially a length, typically derived from `SvPV`. The C function is responsible for copying the data into an arena if it needs to persist beyond the function call.
*   **Output to Perl:** Functions returning data to Perl should generally return new SVs created via `newSVpv`, `newSVuv`, etc., with their ownership passed to Perl. Memory for these SVs is managed by Perl's aREFCNT system.
