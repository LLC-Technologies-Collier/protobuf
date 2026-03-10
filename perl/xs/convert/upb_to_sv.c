
#include "xs/convert/upb_to_sv.h"
#include "xs/protobuf.h"
#include "xs/protobuf/message.h" // Include the new header
#include "upb/reflection/def.h"
#include "upb/wire/types.h"
#include "upb/message/array.h"
#include <stdint.h>
#include <stdio.h>

static SV* convert_singular_upb_to_sv(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv) {
    upb_FieldType type = upb_FieldDef_Type(f);
    switch (type) {
        case kUpb_FieldType_Int32:
        case kUpb_FieldType_SInt32:
        case kUpb_FieldType_SFixed32:
        case kUpb_FieldType_Enum:
            return newSViv(val->int32_val);
        case kUpb_FieldType_String:
        case kUpb_FieldType_Bytes: {
            return newSVpvn(val->str_val.data, val->str_val.size);
        }
        case kUpb_FieldType_Bool:
            return newSVsv(val->bool_val ? &PL_sv_yes : &PL_sv_no);
        case kUpb_FieldType_Float:
            return newSVnv((double)val->float_val);
        case kUpb_FieldType_Double:
            return newSVnv(val->double_val);
        case kUpb_FieldType_UInt32:
        case kUpb_FieldType_Fixed32:
            return newSVuv(val->uint32_val);
        case kUpb_FieldType_Int64:
        case kUpb_FieldType_SInt64:
        case kUpb_FieldType_SFixed64:
            return newSViv(val->int64_val); // TODO: Handle BigInt
        case kUpb_FieldType_UInt64:
        case kUpb_FieldType_Fixed64:
            return newSVuv(val->uint64_val); // TODO: Handle BigInt
        case kUpb_FieldType_Message:
        case kUpb_FieldType_Group: {
            const upb_MessageDef *mdef = upb_FieldDef_MessageSubDef(f);
            if (!mdef) {
                const char *field_name = upb_FieldDef_Name(f);
                croak("PerlUpb_UpbToSv: Could not get MessageDef for field %s", field_name);
            }
            return PerlUpb_WrapMessage(aTHX_ val->msg_val, mdef, parent_arena_sv);
        }
        default:
            croak("Unknown upb field type: %d", type);
            return newSV(0); // Not reached
    }
}

SV *PerlUpb_UpbToSv(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv) {
    if (!f) {
        croak("PerlUpb_UpbToSv: upb_FieldDef was NULL");
        return newSV(0); // Not reached
    }
    if (!val) {
        return newSV(0); // Return undef
    }

    if (upb_FieldDef_IsRepeated(f)) {
        const upb_Array *arr = val->array_val;
        if (!arr) return newRV_noinc((SV*)newAV()); // Empty array

        size_t size = upb_Array_Size(arr);
        AV *av = newAV();
        for (size_t i = 0; i < size; ++i) {
            upb_MessageValue item_val = upb_Array_Get(arr, i);
            SV *item_sv = convert_singular_upb_to_sv(aTHX_ &item_val, f, parent_arena_sv);
            av_push(av, item_sv);
        }
        return newRV_noinc((SV*)av);
    } else {
        return convert_singular_upb_to_sv(aTHX_ val, f, parent_arena_sv);
    }
}
