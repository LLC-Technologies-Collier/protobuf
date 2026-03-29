#ifndef PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_
#define PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/descriptor/base.h"
#include "upb/reflection/def.h"

const char* PerlUpb_MessageDef_FullName(pTHX_ const upb_MessageDef *m);
const char* PerlUpb_MessageDef_Name(pTHX_ const upb_MessageDef *m);
int PerlUpb_MessageDef_FieldCount(pTHX_ const upb_MessageDef *m);
const upb_FieldDef* PerlUpb_MessageDef_FindFieldByNumber(pTHX_ const upb_MessageDef *m, uint32_t i);
const upb_FieldDef* PerlUpb_MessageDef_FindFieldByName(pTHX_ const upb_MessageDef *m, const char *name);
int PerlUpb_MessageDef_OneofCount(pTHX_ const upb_MessageDef *m);
int PerlUpb_MessageDef_RealOneofCount(pTHX_ const upb_MessageDef *m);
int PerlUpb_MessageDef_NestedMessageCount(pTHX_ const upb_MessageDef *m);
int PerlUpb_MessageDef_NestedEnumCount(pTHX_ const upb_MessageDef *m);
int PerlUpb_MessageDef_NestedExtensionCount(pTHX_ const upb_MessageDef *m);
const upb_FileDef* PerlUpb_MessageDef_File(pTHX_ const upb_MessageDef *m);
const upb_MessageDef* PerlUpb_MessageDef_ContainingType(pTHX_ const upb_MessageDef *m);
const upb_FieldDef* PerlUpb_MessageDef_Field(pTHX_ const upb_MessageDef *m, int i);
const upb_OneofDef* PerlUpb_MessageDef_Oneof(pTHX_ const upb_MessageDef *m, int i);
const upb_MessageDef* PerlUpb_MessageDef_NestedMessage(pTHX_ const upb_MessageDef *m, int i);
const upb_EnumDef* PerlUpb_MessageDef_NestedEnum(pTHX_ const upb_MessageDef *m, int i);
const upb_FieldDef* PerlUpb_MessageDef_NestedExtension(pTHX_ const upb_MessageDef *m, int i);
bool PerlUpb_MessageDef_IsMapEntry(pTHX_ const upb_MessageDef *m);
bool PerlUpb_MessageDef_IsMessageSet(pTHX_ const upb_MessageDef *m);

SV* PerlUpb_MessageDef_GetWrapper(pTHX_ const upb_MessageDef *m);
const upb_MessageDef* PerlUpb_MessageDef_GetMessage(pTHX_ SV *sv);

#endif // PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_
