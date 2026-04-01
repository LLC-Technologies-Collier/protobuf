#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/message.h"

const upb_FieldDef* PerlUpb_MessageDef_FindFieldByNameWithSize(pTHX_ const upb_MessageDef *m, const char *name, size_t len) {
    // Manual iteration to debug UPB lookup
    int count = upb_MessageDef_FieldCount(m);
    for (int i = 0; i < count; i++) {
        const upb_FieldDef* f = upb_MessageDef_Field(m, i);
        const char* f_name = upb_FieldDef_Name(f);
        if (strlen(f_name) == len && strncmp(f_name, name, len) == 0) {
            return f;
        }
    }
    return NULL;
}

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_MessageDef_GetWrapper(pTHX_ const upb_MessageDef *m) {
    RETURN_CACHED_OR_CREATE_BLESSED(m, "Protobuf::Descriptor::MessageDef");
}

const upb_MessageDef* PerlUpb_MessageDef_GetMessage(pTHX_ SV *sv) {
    EXTRACT_CACHED_DESCRIPTOR(upb_MessageDef, sv, "Protobuf::Descriptor::MessageDef");
}
