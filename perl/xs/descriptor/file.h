#ifndef PERL_PROTOBUF_DESCRIPTOR_FILE_H_
#define PERL_PROTOBUF_DESCRIPTOR_FILE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/descriptor/base.h"
#include "upb/reflection/def.h"

const char* PerlUpb_FileDef_Name(pTHX_ const upb_FileDef *f);
const char* PerlUpb_FileDef_Package(pTHX_ const upb_FileDef *f);
int PerlUpb_FileDef_DependencyCount(pTHX_ const upb_FileDef *f);
const upb_FileDef* PerlUpb_FileDef_Dependency(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_PublicDependencyCount(pTHX_ const upb_FileDef *f);
const upb_FileDef* PerlUpb_FileDef_PublicDependency(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_WeakDependencyCount(pTHX_ const upb_FileDef *f);
const upb_FileDef* PerlUpb_FileDef_WeakDependency(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_TopLevelMessageCount(pTHX_ const upb_FileDef *f);
const upb_MessageDef* PerlUpb_FileDef_TopLevelMessage(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_TopLevelEnumCount(pTHX_ const upb_FileDef *f);
const upb_EnumDef* PerlUpb_FileDef_TopLevelEnum(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_TopLevelExtensionCount(pTHX_ const upb_FileDef *f);
const upb_FieldDef* PerlUpb_FileDef_TopLevelExtension(pTHX_ const upb_FileDef *f, int i);
int PerlUpb_FileDef_ServiceCount(pTHX_ const upb_FileDef *f);
const upb_ServiceDef* PerlUpb_FileDef_Service(pTHX_ const upb_FileDef *f, int i);
const upb_DefPool* PerlUpb_FileDef_Pool(pTHX_ const upb_FileDef *f);

#endif // PERL_PROTOBUF_DESCRIPTOR_FILE_H_
