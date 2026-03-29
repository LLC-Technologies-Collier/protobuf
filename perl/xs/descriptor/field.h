#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#ifndef PERLUPB_DESCRIPTOR_FIELD_H
#define PERLUPB_DESCRIPTOR_FIELD_H

#include "upb/reflection/def.h"
#include "upb/base/descriptor_constants.h"
#include "EXTERN.h"
#include "perl.h"

// upb_FieldDef Accessors
const char* PerlUpb_FieldDef_Name(pTHX_ const upb_FieldDef* f);
const char* PerlUpb_FieldDef_FullName(pTHX_ const upb_FieldDef* f);
uint32_t PerlUpb_FieldDef_Index(pTHX_ const upb_FieldDef* f);
upb_FieldType PerlUpb_FieldDef_Type(pTHX_ const upb_FieldDef* f);
upb_Label PerlUpb_FieldDef_Label(pTHX_ const upb_FieldDef* f);
uint32_t PerlUpb_FieldDef_Number(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_IsRequired(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_IsOptional(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_IsRepeated(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_IsPacked(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_IsMap(pTHX_ const upb_FieldDef* f);
const upb_MessageDef* PerlUpb_FieldDef_ContainingType(pTHX_ const upb_FieldDef* f);
const upb_OneofDef* PerlUpb_FieldDef_ContainingOneof(pTHX_ const upb_FieldDef* f);
const upb_MessageDef* PerlUpb_FieldDef_MessageSubDef(pTHX_ const upb_FieldDef* f);
const upb_EnumDef* PerlUpb_FieldDef_EnumSubDef(pTHX_ const upb_FieldDef* f);
bool PerlUpb_FieldDef_HasPresence(pTHX_ const upb_FieldDef *f);

SV* PerlUpb_FieldDef_GetWrapper(pTHX_ const upb_FieldDef *f);
const upb_FieldDef* PerlUpb_FieldDef_GetField(pTHX_ SV *sv);

#endif // PERL_PROTOBUF_DESCRIPTOR_FIELD_H_
