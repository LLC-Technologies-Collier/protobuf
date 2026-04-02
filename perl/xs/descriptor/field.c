#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/field.h"
#include "upb/base/descriptor_constants.h"

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_FieldDef_GetWrapper(pTHX_ const upb_FieldDef *f) {
    RETURN_CACHED_OR_CREATE_BLESSED(f, "Protobuf::Descriptor::Field");
}

const upb_FieldDef* PerlUpb_FieldDef_GetField(pTHX_ SV *sv) {
    EXTRACT_CACHED_DESCRIPTOR(upb_FieldDef, sv, "Protobuf::Descriptor::Field");
}

SV* PerlUpb_FieldDef_Name(pTHX_ const upb_FieldDef *f) {
    if (!f) return newSV(0);
    const char* name = upb_FieldDef_Name(f);
    return name ? newSVpv(name, 0) : newSV(0);
}

int PerlUpb_FieldDef_Type(pTHX_ const upb_FieldDef *f) {
    if (!f) return 0;
    return upb_FieldDef_Type(f);
}

int PerlUpb_FieldDef_Label(pTHX_ const upb_FieldDef *f) {
    if (!f) return 0;
    return upb_FieldDef_Label(f);
}
