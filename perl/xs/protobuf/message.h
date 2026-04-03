#ifndef PERL_PROTOBUF_MESSAGE_H_
#define PERL_PROTOBUF_MESSAGE_H_

#include "xs/protobuf.h"
#include "upb/reflection/def.h"

// TODO: Implement these functions
SV* PerlUpb_WrapMessage(pTHX_ const upb_Message *msg, const upb_MessageDef *mdef, SV *arena_sv);
SV* PerlUpb_MaybeGetMessage(pTHX_ const upb_Message *msg);
void PerlUpb_Message_Free(pTHX_ SV *message_sv);

// Helper functions to extract components from a blessed message object
const upb_Message* PerlUpb_Message_GetMsg(pTHX_ SV* message_sv);
const upb_MessageDef* PerlUpb_Message_GetDef(pTHX_ SV* message_sv);
SV* PerlUpb_Message_GetArena(pTHX_ SV* message_sv);

// Cross-process fingerprint for object identity
SV* PerlUpb_Message_GetFingerprint(pTHX_ SV* message_sv);

#endif // PERL_PROTOBUF_MESSAGE_H_
