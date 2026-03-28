
#ifndef PERLUPB_DESCRIPTOR_ENUM_H
#define PERLUPB_DESCRIPTOR_ENUM_H

#include "upb/reflection/def.h"
#include "EXTERN.h"
#include "perl.h"

const char* PerlUpb_EnumDef_FullName(pTHX_ const upb_EnumDef *e);
const char* PerlUpb_EnumDef_Name(pTHX_ const upb_EnumDef *e);
int PerlUpb_EnumDef_ValueCount(pTHX_ const upb_EnumDef *e);
const upb_EnumValueDef* PerlUpb_EnumDef_Value(pTHX_ const upb_EnumDef *e, int i);

#endif /* PERLUPB_DESCRIPTOR_ENUM_H */
