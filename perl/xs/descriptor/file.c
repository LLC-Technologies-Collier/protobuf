#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/file.h"

const char* PerlUpb_FileDef_Name(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_Name(f);
}

const char* PerlUpb_FileDef_Package(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_Package(f);
}

int PerlUpb_FileDef_DependencyCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_DependencyCount(f);
}

const upb_FileDef* PerlUpb_FileDef_Dependency(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_Dependency(f, i);
}

int PerlUpb_FileDef_PublicDependencyCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_PublicDependencyCount(f);
}

const upb_FileDef* PerlUpb_FileDef_PublicDependency(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_PublicDependency(f, i);
}

int PerlUpb_FileDef_WeakDependencyCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_WeakDependencyCount(f);
}

const upb_FileDef* PerlUpb_FileDef_WeakDependency(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_WeakDependency(f, i);
}

int PerlUpb_FileDef_TopLevelMessageCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_TopLevelMessageCount(f);
}

const upb_MessageDef* PerlUpb_FileDef_TopLevelMessage(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_TopLevelMessage(f, i);
}

int PerlUpb_FileDef_TopLevelEnumCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_TopLevelEnumCount(f);
}

const upb_EnumDef* PerlUpb_FileDef_TopLevelEnum(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_TopLevelEnum(f, i);
}

int PerlUpb_FileDef_TopLevelExtensionCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_TopLevelExtensionCount(f);
}

const upb_FieldDef* PerlUpb_FileDef_TopLevelExtension(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_TopLevelExtension(f, i);
}

int PerlUpb_FileDef_ServiceCount(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_ServiceCount(f);
}

const upb_ServiceDef* PerlUpb_FileDef_Service(pTHX_ const upb_FileDef *f, int i) {
    return upb_FileDef_Service(f, i);
}

const upb_DefPool* PerlUpb_FileDef_Pool(pTHX_ const upb_FileDef *f) {
    return upb_FileDef_Pool(f);
}

#include "xs/protobuf/obj_cache.h"
#include "xs/descriptor/base.h"

SV* PerlUpb_FileDef_GetWrapper(pTHX_ const upb_FileDef *f) {
    RETURN_CACHED_OR_CREATE_BLESSED(f, "Protobuf::Descriptor::File");
}

const upb_FileDef* PerlUpb_FileDef_GetFile(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Descriptor::File")) {
        return NULL;
    }
    return (const upb_FileDef*)SvIV(SvRV(sv));
}
