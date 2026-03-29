#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/enum.h"

const char* PerlUpb_EnumDef_FullName(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_FullName(e); }
const char* PerlUpb_EnumDef_Name(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_Name(e); }
int PerlUpb_EnumDef_ValueCount(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_ValueCount(e); }
const upb_EnumValueDef* PerlUpb_EnumDef_Value(pTHX_ const upb_EnumDef *e, int i) { return upb_EnumDef_Value(e, i); }

#include "xs/protobuf/obj_cache.h"

SV* PerlUpb_EnumDef_GetWrapper(pTHX_ const upb_EnumDef *e) {
    if (!e) return &PL_sv_undef;
    SV* cached = PerlUpb_ObjCache_Get(aTHX_ e);
    if (cached) return cached;

    SV* sv = newSViv((IV)e);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::EnumDescriptor", GV_ADD));
    PerlUpb_ObjCache_Add(aTHX_ e, obj);
    return obj;
}

const upb_EnumDef* PerlUpb_EnumDef_GetEnum(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::EnumDescriptor")) {
        return NULL;
    }
    return (const upb_EnumDef*)SvIV(SvRV(sv));
}
