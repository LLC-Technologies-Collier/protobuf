# Architecture: XS Internals & C API

[TOC]

## Purpose of `libprotobuf_common.so`

`libprotobuf_common.so` is a shared library containing the `upb` core, third-party dependencies (`utf8_range`), and custom C helper functions specifically for this Perl-UPB binding. This library centralizes common logic used across all XS sub-modules (`Arena.so`, `Message.so`, etc.).

## Symbol Visibility and ABI Stability

To ensure ABI stability and prevent symbol collisions, we use a linker version script (`libprotobuf_common.map`) to strictly control which symbols are exported from `libprotobuf_common.so`.

*   **Global Symbols:** Only symbols within the `PerlUpb_*`, `upb_*`, `_upb_*`, `google_*`, and `utf8_range_*` namespaces are exported.
*   **Local Symbols:** All other symbols (including internal helpers and leaked `main` functions from test objects) are hidden from the global namespace using `local: *;`.

This approach ensures that the shared library only exposes its intended public API.

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
