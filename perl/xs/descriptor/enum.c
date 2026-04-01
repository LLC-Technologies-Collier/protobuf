#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/enum.h"

const char* PerlUpb_EnumDef_FullName(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_FullName(e); }
const char* PerlUpb_EnumDef_Name(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_Name(e); }
int PerlUpb_EnumDef_ValueCount(pTHX_ const upb_EnumDef *e) { return upb_EnumDef_ValueCount(e); }
const upb_EnumValueDef* PerlUpb_EnumDef_Value(pTHX_ const upb_EnumDef *e, int i) { return upb_EnumDef_Value(e, i); }

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_EnumDef_GetWrapper(pTHX_ const upb_EnumDef *e) {
    RETURN_CACHED_OR_CREATE_BLESSED(e, "Protobuf::Descriptor::Enum");
}

const upb_EnumDef* PerlUpb_EnumDef_GetEnum(pTHX_ SV *sv) {
    EXTRACT_CACHED_DESCRIPTOR(upb_EnumDef, sv, "Protobuf::Descriptor::Enum");
}
