#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/field.h"
#include "upb/base/descriptor_constants.h"

const char* PerlUpb_FieldDef_Name(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_Name(f); }
const char* PerlUpb_FieldDef_FullName(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_FullName(f); }
uint32_t PerlUpb_FieldDef_Index(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_Index(f); }
upb_FieldType PerlUpb_FieldDef_Type(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_Type(f); }
upb_Label PerlUpb_FieldDef_Label(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_Label(f); }
uint32_t PerlUpb_FieldDef_Number(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_Number(f); }
bool PerlUpb_FieldDef_IsRequired(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_IsRequired(f); }
bool PerlUpb_FieldDef_IsOptional(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_IsOptional(f); }
bool PerlUpb_FieldDef_IsRepeated(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_IsRepeated(f); }
bool PerlUpb_FieldDef_IsPacked(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_IsPacked(f); }
bool PerlUpb_FieldDef_IsMap(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_IsMap(f); }
const upb_MessageDef* PerlUpb_FieldDef_ContainingType(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_ContainingType(f); }
const upb_OneofDef* PerlUpb_FieldDef_ContainingOneof(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_ContainingOneof(f); }
const upb_MessageDef* PerlUpb_FieldDef_MessageSubDef(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_MessageSubDef(f); }
const upb_EnumDef* PerlUpb_FieldDef_EnumSubDef(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_EnumSubDef(f); }
bool PerlUpb_FieldDef_HasPresence(pTHX_ const upb_FieldDef* f) { return upb_FieldDef_HasPresence(f); }

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_FieldDef_GetWrapper(pTHX_ const upb_FieldDef *f) {
    RETURN_CACHED_OR_CREATE_BLESSED(f, "Protobuf::Descriptor::Field");
}

const upb_FieldDef* PerlUpb_FieldDef_GetField(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Descriptor::Field")) {
        return NULL;
    }
    return (const upb_FieldDef*)SvIV(SvRV(sv));
}
