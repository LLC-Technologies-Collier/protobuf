#include "xs/convert/sv_to_upb.h"
#include "xs/protobuf.h"
#include "xs/protobuf/message.h"
#include "xs/repeated/repeated.h"
#include "xs/map/map.h"
#include "t/c/upb-perl-test.h" // Added for cdiag
#include "upb/base/descriptor_constants.h"
#include "upb/reflection/def.h"
#include "upb/message/array.h"
#include "upb/message/copy.h"
#include "upb/message/map.h"
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
            if (SvIOK(sv) || SvNOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                val->int32_val = (int32_t)SvIV(sv);
                return true;
            }
            CROAK_WRONG_TYPE(sv, "an Integer", f);
            return false;
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
            if (SvNOK(sv) || SvIOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                val->float_val = (float)SvNV(sv);
                return true;
            }
            CROAK_WRONG_TYPE(sv, "a Number", f);
            return false;
        case kUpb_FieldType_Double:
            if (SvNOK(sv) || SvIOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                val->double_val = SvNV(sv);
                return true;
            }
            CROAK_WRONG_TYPE(sv, "a Number", f);
            return false;
        case kUpb_FieldType_UInt32:
        case kUpb_FieldType_Fixed32: {
            if (SvUOK(sv) || SvIOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                UV uv = SvUV(sv);
                val->uint32_val = (uint32_t)uv;
                return true;
            }
            CROAK_WRONG_TYPE(sv, "an Unsigned Integer", f);
            return false;
        }
        case kUpb_FieldType_Int64:
        case kUpb_FieldType_SInt64:
        case kUpb_FieldType_SFixed64:
            if (SvIOK(sv) || SvNOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                val->int64_val = (int64_t)SvIV(sv);
                return true;
            }
            CROAK_WRONG_TYPE(sv, "an Integer", f);
            return false;
        case kUpb_FieldType_UInt64:
        case kUpb_FieldType_Fixed64: {
            if (SvUOK(sv) || SvIOK(sv) || (SvPOK(sv) && looks_like_number(sv))) {
                val->uint64_val = (uint64_t)SvUV(sv);
                return true;
            }
            CROAK_WRONG_TYPE(sv, "an Unsigned Integer", f);
            return false;
        }
        case kUpb_FieldType_Enum:
            if (SvIOK(sv)) {
                val->int32_val = SvIV(sv);
                return true;
            } else if (SvPOK(sv)) {
                STRLEN len;
                const char *name = SvPV(sv, len);
                const upb_EnumDef *edef = upb_FieldDef_EnumSubDef(f);
                if (!edef) croak("Missing EnumDef for field '%s'", upb_FieldDef_Name(f));
                
                const upb_EnumValueDef *ev = upb_EnumDef_FindValueByNameWithSize(edef, name, len);
                if (!ev) {
                    croak("Invalid enum name '%s' for field '%s'", name, upb_FieldDef_Name(f));
                }
                val->int32_val = upb_EnumValueDef_Number(ev);
                return true;
            } else {
                CROAK_WRONG_TYPE(sv, "an Integer or Enum Name", f);
            }
            return true;
        case kUpb_FieldType_Message:
        case kUpb_FieldType_Group: {
             const upb_Message* src_msg = PerlUpb_Message_GetMsg(aTHX_ sv);
             if (!src_msg) CROAK_WRONG_TYPE(sv, "a Message object", f);
             
             const upb_MessageDef* target_mdef = upb_FieldDef_MessageSubDef(f);
             const upb_MessageDef* src_mdef = PerlUpb_Message_GetDef(aTHX_ sv);
             if (src_mdef != target_mdef) {
                 croak("Message type mismatch for field '%s': expected %s, got %s",
                       upb_FieldDef_Name(f), upb_MessageDef_FullName(target_mdef),
                       upb_MessageDef_FullName(src_mdef));
             }

             const upb_MiniTable* mt = upb_MessageDef_MiniTable(target_mdef);
             upb_Message* dst_msg = upb_Message_New(mt, arena);
             if (!upb_Message_DeepCopy(dst_msg, src_msg, mt, arena)) {
                 croak("Deep copy of message failed");
             }
             val->msg_val = dst_msg;
             return true;
        }
        default:
            croak("Unknown upb field type: %d", type);
            return false;
    }
    return false; // Should not reach here
}


bool PerlUpb_SvToUpb_Element(pTHX_ SV *sv, const upb_FieldDef *f, upb_MessageValue *val, upb_Arena *arena) {
    if (!f || !val || !arena) return false;
    return convert_singular_sv_to_upb(aTHX_ sv, f, val, arena);
}

bool PerlUpb_SvToUpb(pTHX_ SV *sv, const upb_FieldDef *f, upb_MessageValue *val, upb_Arena *arena) {
    if (!f || !val || !arena) return false;

    if (!SvOK(sv)) {
        croak("Cannot convert undef to non-message type for field '%s'", upb_FieldDef_Name(f));
        return false;
    }

    if (upb_FieldDef_IsRepeated(f)) {
        if (!SvROK(sv)) {
            CROAK_WRONG_TYPE(sv, "a Reference", f);
            return false;
        }

        if (upb_FieldDef_IsMap(f)) {
            if (SvTYPE(SvRV(sv)) != SVt_PVHV) {
                CROAK_WRONG_TYPE(sv, "a Hash Reference", f);
                return false;
            }
            HV* hv = (HV*)SvRV(sv);
            SV* tied_obj = NULL;
            MAGIC* mg = mg_find((SV*)hv, PERL_MAGIC_tied);
            if (mg) {
                tied_obj = SvTIED_obj((SV*)hv, mg);
                if (tied_obj && sv_derived_from(tied_obj, "Protobuf::Internal::Map")) {
                    upb_Map* src_map = PerlUpb_Map_GetMapPtr(aTHX_ tied_obj);
                    const upb_FieldDef* src_f = PerlUpb_Map_GetFieldDef(aTHX_ tied_obj);
                    if (src_map) {
                        if (upb_FieldDef_Type(src_f) != upb_FieldDef_Type(f)) {
                            croak("Type mismatch when copying map field");
                        }

                        const upb_MessageDef* entry_def = upb_FieldDef_MessageSubDef(f);
                        const upb_FieldDef* key_f = upb_MessageDef_FindFieldByNumber(entry_def, 1);
                        const upb_FieldDef* val_f = upb_MessageDef_FindFieldByNumber(entry_def, 2);

                        // Perform deep copy
                        upb_Map* dst_map = upb_Map_New(arena, upb_FieldDef_CType(key_f), upb_FieldDef_CType(val_f));
                        size_t iter = kUpb_Map_Begin;
                        upb_MessageValue k, v;
                        const upb_MiniTable* val_mt = upb_FieldDef_IsSubMessage(val_f) ? upb_MessageDef_MiniTable(upb_FieldDef_MessageSubDef(val_f)) : NULL;

                        while (upb_Map_Next(src_map, &k, &v, &iter)) {
                            if (val_mt) {
                                upb_Message* dst_v = upb_Message_New(val_mt, arena);
                                upb_Message_DeepCopy(dst_v, v.msg_val, val_mt, arena);
                                v.msg_val = dst_v;
                            }
                            upb_Map_Set(dst_map, k, v, arena);
                        }
                        val->map_val = dst_map;
                        return true;
                    }
                }
            }

            // Fallback: iterate over Perl hash
            const upb_MessageDef* entry_def = upb_FieldDef_MessageSubDef(f);
            const upb_FieldDef* key_f = upb_MessageDef_FindFieldByNumber(entry_def, 1);
            const upb_FieldDef* val_f = upb_MessageDef_FindFieldByNumber(entry_def, 2);
            upb_Map* dst_map = upb_Map_New(arena, upb_FieldDef_CType(key_f), upb_FieldDef_CType(val_f));

            hv_iterinit(hv);
            HE* he;
            while ((he = hv_iternext(hv))) {
                SV* key_sv = hv_iterkeysv(he);
                SV* val_sv = hv_iterval(hv, he);

                if (mg && (!val_sv || !SvOK(val_sv))) {
                    dSP; ENTER; SAVETMPS; PUSHMARK(SP);
                    XPUSHs(tied_obj); XPUSHs(sv_2mortal(newSVsv(key_sv))); PUTBACK;
                    call_method("FETCH", G_SCALAR);
                    SPAGAIN; val_sv = POPs; PUTBACK;
                }

                upb_MessageValue k, v;
                if (!convert_singular_sv_to_upb(aTHX_ key_sv, key_f, &k, arena)) return false;
                if (!convert_singular_sv_to_upb(aTHX_ val_sv, val_f, &v, arena)) return false;
                upb_Map_Set(dst_map, k, v, arena);

                if (mg) { FREETMPS; LEAVE; }
            }
            val->map_val = dst_map;
            return true;
        }

        if (SvTYPE(SvRV(sv)) != SVt_PVAV) {
            CROAK_WRONG_TYPE(sv, "an Array Reference", f);
            return false;
        }
        AV *av = (AV*)SvRV(sv);
        SV* tied_obj = NULL;

        // Check for our tied proxy object
        MAGIC* mg = mg_find((SV*)av, PERL_MAGIC_tied);
        if (mg) {
            tied_obj = SvTIED_obj((SV*)av, mg);
            if (tied_obj && sv_derived_from(tied_obj, "Protobuf::Internal::Repeated")) {
                upb_Array* src_arr = PerlUpb_Repeated_GetArray(aTHX_ tied_obj);
                const upb_FieldDef* src_f = PerlUpb_Repeated_GetFieldDef(aTHX_ tied_obj);
                
                if (src_arr) {
                    if (upb_FieldDef_Type(src_f) != upb_FieldDef_Type(f)) {
                        croak("Type mismatch when copying repeated field '%s': expected %d, got %d",
                              upb_FieldDef_Name(f), upb_FieldDef_Type(f), upb_FieldDef_Type(src_f));
                    }

                    size_t n = upb_Array_Size(src_arr);
                    upb_Array* dst_arr = upb_Array_New(arena, upb_FieldDef_CType(f));
                    if (!upb_Array_Resize(dst_arr, n, arena)) croak("Failed to resize destination array");

                    for (size_t i = 0; i < n; i++) {
                        upb_MessageValue item = upb_Array_Get(src_arr, i);
                        if (upb_FieldDef_IsSubMessage(f)) {
                            const upb_MessageDef* mdef = upb_FieldDef_MessageSubDef(f);
                            const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
                            upb_Message* dst_msg = upb_Message_New(mt, arena);
                            if (!upb_Message_DeepCopy(dst_msg, item.msg_val, mt, arena)) {
                                croak("Failed to deep copy message element %zu", i);
                            }
                            item.msg_val = dst_msg;
                        }
                        upb_Array_Set(dst_arr, i, item);
                    }
                    val->array_val = dst_arr;
                    return true;
                }
            }
        }

        // Fallback: iterate over the array elements
        I32 max_idx = av_len(av);
        size_t num_elements = max_idx + 1;

        upb_CType ctype = upb_FieldDef_CType(f);
        upb_Array *arr = upb_Array_New(arena, ctype);
        if (!arr) {
            croak("Failed to create upb_Array for field '%s'", upb_FieldDef_Name(f));
            return false;
        }

        for (I32 i = 0; i < (I32)num_elements; ++i) {
            SV **elem_sv_ptr = av_fetch(av, i, 0);
            SV *elem_sv = (elem_sv_ptr) ? *elem_sv_ptr : NULL;
            
            // If it's a tied array and av_fetch returned NULL or it's empty, we must use the tied interface
            if (mg && (!elem_sv || !SvOK(elem_sv))) {
                dSP;
                ENTER;
                SAVETMPS;
                PUSHMARK(SP);
                XPUSHs(tied_obj);
                XPUSHs(sv_2mortal(newSViv(i)));
                PUTBACK;
                call_method("FETCH", G_SCALAR);
                SPAGAIN;
                elem_sv = POPs;
                PUTBACK;
            }

            if (!elem_sv) {
                croak("Error fetching element %d from array for field '%s'", (int)i, upb_FieldDef_Name(f));
                return false;
            }

            upb_MessageValue item_val;
            if (!convert_singular_sv_to_upb(aTHX_ elem_sv, f, &item_val, arena)) {
                return false; // Error already croaked
            }
            if (!upb_Array_Append(arr, item_val, arena)) {
                croak("Failed to append to upb_Array for field '%s'", upb_FieldDef_Name(f));
                return false;
            }
            
            if (mg) {
                FREETMPS;
                LEAVE;
            }
        }
        val->array_val = arr;
        return true;
    } else {
        return convert_singular_sv_to_upb(aTHX_ sv, f, val, arena);
    }
}
