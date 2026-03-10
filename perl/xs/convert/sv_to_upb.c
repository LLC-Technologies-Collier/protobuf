#include "xs/convert/sv_to_upb.h"
#include "xs/protobuf.h"
#include "t/c/upb-perl-test.h" // Added for cdiag
#include "upb/base/descriptor_constants.h"
#include "upb/reflection/def.h"
#include "upb/message/array.h"
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

#define CROAK_WRONG_TYPE(sv, expected, field) \
    croak("Expected %s for field '%s', but got %s", expected, upb_FieldDef_Name(field), sv_reftype(sv, TRUE))

static bool convert_singular_sv_to_upb(pTHX_ SV *sv, const upb_FieldDef *f, upb_MessageValue *val, upb_Arena *arena) {
    upb_FieldType type = upb_FieldDef_Type(f);
    switch (type) {
        case kUpb_FieldType_Int32:
        case kUpb_FieldType_SInt32:
        case kUpb_FieldType_SFixed32:
            if (!SvIOK(sv)) CROAK_WRONG_TYPE(sv, "an Integer", f);
            val->int32_val = SvIV(sv);
            return true;
        case kUpb_FieldType_String:
        case kUpb_FieldType_Bytes: {
            if (!SvPOK(sv)) CROAK_WRONG_TYPE(sv, "a String or Bytes", f);
            STRLEN len;
            const char *ptr = SvPV(sv, len);
            char *new_ptr = (char *)upb_Arena_Malloc(arena, len);
            if (!new_ptr) croak("Arena allocation failed for string/bytes");
            memcpy(new_ptr, ptr, len);
            val->str_val = upb_StringView_FromDataAndSize(new_ptr, len);
            return true;
        }
        case kUpb_FieldType_Bool:
            val->bool_val = SvTRUE(sv);
            return true;
        case kUpb_FieldType_Float:
            if (!SvNOK(sv) && !SvIOK(sv)) CROAK_WRONG_TYPE(sv, "a Number", f);
            val->float_val = (float)SvNV(sv);
            return true;
        case kUpb_FieldType_Double:
            if (!SvNOK(sv) && !SvIOK(sv)) CROAK_WRONG_TYPE(sv, "a Number", f);
            val->double_val = SvNV(sv);
            return true;
        case kUpb_FieldType_UInt32:
        case kUpb_FieldType_Fixed32: {
            if (SvUOK(sv)) {
                val->uint32_val = SvUVX(sv);
            } else if (SvIOK(sv)) {
                IV iv = SvIVX(sv);
                if (iv < 0) {
                    croak("Integer %" IVdf " out of range for uint32 field '%s'", iv, upb_FieldDef_Name(f));
                }
                val->uint32_val = (uint32_t)iv;
            } else {
                 CROAK_WRONG_TYPE(sv, "an Unsigned Integer", f);
            }
            return true;
        }
        case kUpb_FieldType_Int64:
        case kUpb_FieldType_SInt64:
        case kUpb_FieldType_SFixed64:
            if (!SvIOK(sv)) CROAK_WRONG_TYPE(sv, "an Integer", f); // TODO: Handle BigInt
            val->int64_val = SvIVX(sv);
            return true;
        case kUpb_FieldType_UInt64:
        case kUpb_FieldType_Fixed64: {
             if (SvUOK(sv)) {
                val->uint64_val = SvUVX(sv);
            } else if (SvIOK(sv)) {
                IV iv = SvIVX(sv);
                if (iv < 0) {
                    croak("Cannot convert negative integer to uint64 for field '%s'", upb_FieldDef_Name(f));
                    return false;
                }
                val->uint64_val = (uint64_t)iv;
            } else {
                CROAK_WRONG_TYPE(sv, "an Unsigned Integer", f);
            }
            return true;
        }
        case kUpb_FieldType_Enum:
            if (!SvIOK(sv)) CROAK_WRONG_TYPE(sv, "an Integer", f);
            // TODO: Add enum range validation?
            val->int32_val = SvIV(sv);
            return true;
        case kUpb_FieldType_Message:
        case kUpb_FieldType_Group:
             croak("Message type conversion not yet implemented in sv_to_upb");
             return false;
        default:
            croak("Unknown upb field type: %d", type);
            return false;
    }
    return false; // Should not reach here
}

bool PerlUpb_SvToUpb(pTHX_ SV *sv, const upb_FieldDef *f, upb_MessageValue *val, upb_Arena *arena) {
    // log something here? ("PerlUpb_SvToUpb ENTERED for field %s", f ? upb_FieldDef_Name(f) : "NULL_FIELD");
    if (!f) {
        croak("PerlUpb_SvToUpb: upb_FieldDef was NULL");
        return false;
    }
    if (!val) {
        croak("PerlUpb_SvToUpb: val was NULL");
        return false;
    }
    if (!arena) {
        croak("PerlUpb_SvToUpb: arena was NULL");
        return false;
    }

    if (!SvOK(sv)) {
        croak("Cannot convert undef to non-message type for field '%s'", upb_FieldDef_Name(f));
        return false;
    }

    if (upb_FieldDef_IsRepeated(f)) {
        if (SvROK(sv)) {
        }
        if (!SvROK(sv) || SvTYPE(SvRV(sv)) != SVt_PVAV) {
            CROAK_WRONG_TYPE(sv, "an Array Reference", f);
            return false;
        }
        AV *av = (AV*)SvRV(sv);
        I32 max_idx = av_len(av);
        size_t num_elements = max_idx + 1;

        upb_CType ctype = upb_FieldDef_CType(f);
        upb_Array *arr = upb_Array_New(arena, ctype);
        if (!arr) {
            croak("Failed to create upb_Array for field '%s'", upb_FieldDef_Name(f));
            return false;
        }

        for (I32 i = 0; i < num_elements; ++i) {
            if (i >= num_elements) croak("Loop invariant failed: i >= num_elements");
            SV **elem_sv = av_fetch(av, i, 0);
            if (!elem_sv || !*elem_sv) {
                croak("Error fetching element %d from array for field '%s'", (int)i, upb_FieldDef_Name(f));
                return false;
            }
            upb_MessageValue item_val;
            if (!convert_singular_sv_to_upb(aTHX_ *elem_sv, f, &item_val, arena)) {
                return false; // Error already croaked
            }
            if (!upb_Array_Append(arr, item_val, arena)) {
                croak("Failed to append to upb_Array for field '%s'", upb_FieldDef_Name(f));
                return false;
            }
        }
        val->array_val = arr;
        return true;
    } else {
        return convert_singular_sv_to_upb(aTHX_ sv, f, val, arena);
    }
}
