#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/oneof.h"

const char* PerlUpb_OneofDef_FullName(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_FullName(o);
}

const char* PerlUpb_OneofDef_Name(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_Name(o);
}

int PerlUpb_OneofDef_FieldCount(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_FieldCount(o);
}

const upb_FieldDef* PerlUpb_OneofDef_Field(pTHX_ const upb_OneofDef *o, int i) {
    return upb_OneofDef_Field(o, i);
}

const upb_MessageDef* PerlUpb_OneofDef_ContainingType(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_ContainingType(o);
}

uint32_t PerlUpb_OneofDef_Index(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_Index(o);
}

bool PerlUpb_OneofDef_IsSynthetic(pTHX_ const upb_OneofDef *o) {
    return upb_OneofDef_IsSynthetic(o);
}

const upb_FieldDef* PerlUpb_OneofDef_LookupName(pTHX_ const upb_OneofDef *o, const char *name) {
    return upb_OneofDef_LookupName(o, name);
}

const upb_FieldDef* PerlUpb_OneofDef_LookupNumber(pTHX_ const upb_OneofDef *o, uint32_t num) {
    return upb_OneofDef_LookupNumber(o, num);
}

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_OneofDef_GetWrapper(pTHX_ const upb_OneofDef *o) {
    RETURN_CACHED_OR_CREATE_BLESSED(o, "Protobuf::Descriptor::OneofDef");
}

const upb_OneofDef* PerlUpb_OneofDef_GetOneof(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Descriptor::OneofDef")) {
        return NULL;
    }
    return (const upb_OneofDef*)SvIV(SvRV(sv));
}
