# XS File Organization

_Status: Partially Implemented_

The XS code for the `Protobuf` Perl module is organized into several files, leveraging `XSMULTI => 1` in `Makefile.PL`.

-   **`lib/Protobuf/Descriptor.xs`:** Wraps `upb_MessageDef`, `upb_EnumDef`, etc.
-   **`lib/Protobuf/DescriptorPool.xs`:** Wraps `upb_DefPool`.
-   **`lib/Protobuf/Message.xs`:** (Planned) XSUBs for `Protobuf::Message` operations.
-   **`lib/Protobuf/Arena.xs`:** (Planned) Wrapper around `upb_Arena`.
-   **`xs/types.c`:** Contains type conversion functions (e.g., `sv_to_upb_value`, `upb_value_to_sv`).
-   **`xs/types.h`:** Header for `xs/types.c`.

This structure keeps XSUBs close to their corresponding Perl modules.