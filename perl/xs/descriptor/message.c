#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/message.h"

const char* PerlUpb_MessageDef_FullName(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_FullName(m);
}

const char* PerlUpb_MessageDef_Name(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_Name(m);
}

int PerlUpb_MessageDef_FieldCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_FieldCount(m);
}

const upb_FieldDef* PerlUpb_MessageDef_FindFieldByNumber(pTHX_ const upb_MessageDef *m, uint32_t i) {
    return upb_MessageDef_FindFieldByNumber(m, i);
}

const upb_FieldDef* PerlUpb_MessageDef_FindFieldByName(pTHX_ const upb_MessageDef *m, const char *name) {
    return upb_MessageDef_FindFieldByName(m, name);
}

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


int PerlUpb_MessageDef_OneofCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_OneofCount(m);
}

int PerlUpb_MessageDef_RealOneofCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_RealOneofCount(m);
}

int PerlUpb_MessageDef_NestedMessageCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_NestedMessageCount(m);
}

int PerlUpb_MessageDef_NestedEnumCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_NestedEnumCount(m);
}

int PerlUpb_MessageDef_NestedExtensionCount(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_NestedExtensionCount(m);
}

const upb_FileDef* PerlUpb_MessageDef_File(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_File(m);
}

const upb_MessageDef* PerlUpb_MessageDef_ContainingType(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_ContainingType(m);
}

const upb_FieldDef* PerlUpb_MessageDef_Field(pTHX_ const upb_MessageDef *m, int i) {
    return upb_MessageDef_Field(m, i);
}

const upb_OneofDef* PerlUpb_MessageDef_Oneof(pTHX_ const upb_MessageDef *m, int i) {
    return upb_MessageDef_Oneof(m, i);
}

const upb_MessageDef* PerlUpb_MessageDef_NestedMessage(pTHX_ const upb_MessageDef *m, int i) {
    return upb_MessageDef_NestedMessage(m, i);
}

const upb_EnumDef* PerlUpb_MessageDef_NestedEnum(pTHX_ const upb_MessageDef *m, int i) {
    return upb_MessageDef_NestedEnum(m, i);
}

const upb_FieldDef* PerlUpb_MessageDef_NestedExtension(pTHX_ const upb_MessageDef *m, int i) {
    return upb_MessageDef_NestedExtension(m, i);
}

bool PerlUpb_MessageDef_IsMapEntry(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_IsMapEntry(m);
}

bool PerlUpb_MessageDef_IsMessageSet(pTHX_ const upb_MessageDef *m) {
    return upb_MessageDef_IsMessageSet(m);
}

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_MessageDef_GetWrapper(pTHX_ const upb_MessageDef *m) {
    RETURN_CACHED_OR_CREATE_BLESSED(m, "Protobuf::Descriptor::MessageDef");
}

const upb_MessageDef* PerlUpb_MessageDef_GetMessage(pTHX_ SV *sv) {
    EXTRACT_CACHED_DESCRIPTOR(upb_MessageDef, sv, "Protobuf::Descriptor::MessageDef");
}
