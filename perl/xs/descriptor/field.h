#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#ifndef PERLUPB_DESCRIPTOR_FIELD_H
#define PERLUPB_DESCRIPTOR_FIELD_H

#include "upb/reflection/def.h"
#include "upb/base/descriptor_constants.h"
#include "EXTERN.h"
#include "perl.h"

SV* PerlUpb_FieldDef_GetWrapper(pTHX_ const upb_FieldDef *f);
const upb_FieldDef* PerlUpb_FieldDef_GetField(pTHX_ SV *sv);

SV* PerlUpb_FieldDef_Name(pTHX_ const upb_FieldDef *f);
int PerlUpb_FieldDef_Type(pTHX_ const upb_FieldDef *f);
int PerlUpb_FieldDef_Label(pTHX_ const upb_FieldDef *f);

#endif // PERL_PROTOBUF_DESCRIPTOR_FIELD_H_
