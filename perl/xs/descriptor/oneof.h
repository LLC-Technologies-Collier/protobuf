#ifndef PERL_PROTOBUF_DESCRIPTOR_ONEOF_H_
#define PERL_PROTOBUF_DESCRIPTOR_ONEOF_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/descriptor/base.h"
#include "upb/reflection/def.h"

const char* PerlUpb_OneofDef_FullName(pTHX_ const upb_OneofDef *o);
const char* PerlUpb_OneofDef_Name(pTHX_ const upb_OneofDef *o);
int PerlUpb_OneofDef_FieldCount(pTHX_ const upb_OneofDef *o);
const upb_FieldDef* PerlUpb_OneofDef_Field(pTHX_ const upb_OneofDef *o, int i);
const upb_MessageDef* PerlUpb_OneofDef_ContainingType(pTHX_ const upb_OneofDef *o);
uint32_t PerlUpb_OneofDef_Index(pTHX_ const upb_OneofDef *o);
bool PerlUpb_OneofDef_IsSynthetic(pTHX_ const upb_OneofDef *o);
const upb_FieldDef* PerlUpb_OneofDef_LookupName(pTHX_ const upb_OneofDef *o, const char *name);
const upb_FieldDef* PerlUpb_OneofDef_LookupNumber(pTHX_ const upb_OneofDef *o, uint32_t num);

#endif // PERL_PROTOBUF_DESCRIPTOR_ONEOF_H_
