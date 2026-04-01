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
