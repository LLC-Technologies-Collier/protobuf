#ifndef PERL_PROTOBUF_UPB_TO_SV_H_
#define PERL_PROTOBUF_UPB_TO_SV_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/message/value.h"
#include "upb/reflection/def.h"

SV *PerlUpb_UpbToSv(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv);

// Converts a singular upb_MessageValue to a Perl SV, even if the field is repeated.
// Used for elements of repeated fields or maps.
SV *PerlUpb_UpbToSv_Element(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *parent_arena_sv);


#endif // PERL_PROTOBUF_UPB_TO_SV_H_