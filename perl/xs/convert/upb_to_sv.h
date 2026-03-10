#ifndef PERL_PROTOBUF_UPB_TO_SV_H_
#define PERL_PROTOBUF_UPB_TO_SV_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/message/value.h"
#include "upb/reflection/def.h"

SV *PerlUpb_UpbToSv(pTHX_ const upb_MessageValue *val, const upb_FieldDef *f, SV *arena_sv);

#endif // PERL_PROTOBUF_UPB_TO_SV_H_