#ifndef PERLUPB_DESCRIPTOR_ENUM_H
#define PERLUPB_DESCRIPTOR_ENUM_H

#include "upb/reflection/def.h"
#include "EXTERN.h"
#include "perl.h"

SV* PerlUpb_EnumDef_GetWrapper(pTHX_ const upb_EnumDef *e);
const upb_EnumDef* PerlUpb_EnumDef_GetEnum(pTHX_ SV *sv);

#endif /* PERLUPB_DESCRIPTOR_ENUM_H */
