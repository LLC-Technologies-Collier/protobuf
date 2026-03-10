#ifndef PERL_PROTOBUF_SV_TO_UPB_H_
#define PERL_PROTOBUF_SV_TO_UPB_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/message/value.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"

bool PerlUpb_SvToUpb(pTHX_ SV *sv, const upb_FieldDef *f, upb_MessageValue *val, upb_Arena *arena);

#endif // PERL_PROTOBUF_SV_TO_UPB_H_