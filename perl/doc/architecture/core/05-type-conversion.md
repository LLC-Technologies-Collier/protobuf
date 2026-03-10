# Type Conversion

Marshalling data between Perl's Scalar Values (SVs) and `upb`'s C types (a `upb_MessageValue` union) is a critical function of the XS layer.

## Key Conversions

XS functions will be needed to handle conversions for each Protocol Buffer type:

| Proto Type     | C Type (in `upb_MessageValue`) | Perl Type (SV)         | Conversion Notes                                                                 |
|----------------|--------------------------------|------------------------|----------------------------------------------------------------------------------|
| int32, sint32  | `int32_t`                      | IV                     | Check for overflow.                                                              |
| uint32, fixed32| `uint32_t`                     | UV                     | Check for overflow.                                                              |
| int64, sint64  | `int64_t`                      | IV (if fits), string   | Use functions like `SvIV`, `SvUIV`, potentially lossy for very large numbers.    |
| uint64, fixed64| `uint64_t`                     | UV (if fits), string   | Use functions like `SvUV`, potentially lossy.                                    |
| float          | `float`                        | NV                     | Standard float/double conversion.                                                |
| double         | `double`                       | NV                     | Standard float/double conversion.                                                |
| bool           | `bool`                         | bool                   | `SvTRUE` / `SvFALSE`.                                                            |
| string         | `upb_StringView`               | PV (string)            | Ensure UTF-8 encoding. Data copied into the arena.                               |
| bytes          | `upb_StringView`               | PV (string)            | No encoding checks. Data copied into the arena.                                |
| enum           | `int32_t`                      | IV                     | Integer value of the enum.                                                       |
| message        | `const upb_Message*`           | Blessed Ref            | Pointer to the `upb_Message` instance. Arena fusing is critical.               |
| repeated       | `const upb_Array*`             | `Protobuf::RepeatedField`| Perl wrapper object.                                                             |
| map            | `const upb_Map*`               | `Protobuf::Map`        | Perl wrapper object.                                                             |

## XS Implementation

-   A series of functions like `sv_to_upb_T()` and `upb_to_sv_T()` will be created for each type `T`.
-   These functions will handle type checking on the Perl side (e.g., using `SvIOK`, `SvNOK`, `SvPOK`).
-   String/bytes data from Perl SVs will be copied into the `upb_Arena` to ensure its lifetime is tied to the message.
-   When converting from `upb` to Perl, new SVs will be created. For strings and bytes, the SV will likely copy the data from the arena.

## Encoding

-   Perl strings used for Protobuf `string` fields MUST be valid UTF-8. XS code should validate this, potentially using `sv_utf8_check()` and `SvUTF8()`.
-   Data for `bytes` fields will be treated as raw octets.

This approach is similar to the `convert.c` files found in the Ruby and PHP extensions, which contain a suite of functions for bidirectional type marshalling.
