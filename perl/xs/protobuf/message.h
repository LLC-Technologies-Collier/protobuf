#ifndef PERL_PROTOBUF_MESSAGE_H_
#define PERL_PROTOBUF_MESSAGE_H_

#include "xs/protobuf.h"
#include "upb/reflection/def.h"

// TODO: Implement these functions
SV* PerlUpb_WrapMessage(pTHX_ const upb_Message *msg, const upb_MessageDef *mdef, SV *arena_sv);
SV* PerlUpb_MaybeGetMessage(pTHX_ const upb_Message *msg);

#endif // PERL_PROTOBUF_MESSAGE_H_
