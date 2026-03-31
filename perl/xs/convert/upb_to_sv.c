
#include "xs/convert/upb_to_sv.h"
#include "xs/protobuf.h"
#include "xs/protobuf/message.h"
#include "xs/map/map.h"
#include "xs/repeated/repeated.h"
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

SV* PerlUpb_UpbToSv_Element(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv) {
    if (!f || !val) return newSV(0);
    return convert_singular_upb_to_sv(aTHX_ val, f, parent_arena_sv);
}

SV *PerlUpb_UpbToSv(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv) {
    if (!f) {
        croak("PerlUpb_UpbToSv: upb_FieldDef was NULL");
        return newSV(0); // Not reached
    }
    if (!val) {
        return newSV(0); // Return undef
    }

    if (upb_FieldDef_IsMap(f)) {
        upb_Map *map = (upb_Map*)val->map_val;
        if (!map) return newSV(0); 
        return PerlUpb_Map_New(aTHX_ map, f, parent_arena_sv);
    }

    if (upb_FieldDef_IsRepeated(f)) {
        upb_Array *arr = (upb_Array*)val->array_val;
        if (!arr) return newSV(0); 
        return PerlUpb_Repeated_New(aTHX_ arr, f, parent_arena_sv);
    } else {
        return convert_singular_upb_to_sv(aTHX_ val, f, parent_arena_sv);
    }
}
