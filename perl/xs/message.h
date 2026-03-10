#ifndef PERL_MESSAGE_H_
#define PERL_MESSAGE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/message/message.h"
#include "perl/xs/message/meta.h"
#include "perl/xs/message/access.h"
#include "perl/xs/message/serialize.h"
#include "perl/xs/message/compare.h"
#include "perl/xs/message/init.h"
#include "perl/xs/message/wkt.h"

// Top-level init
bool PerlUpb_InitMessage(pTHX_ SV* module);

// Forward declaration needed by other parts like convert/upb_to_sv.c
struct upb_MessageDef;
struct upb_Arena;
struct upb_Message;
SV *PerlUpb_Message_Get(pTHX_ const struct upb_Message *msg, const struct upb_MessageDef *m, SV *arena_sv);

#endif  // PERL_MESSAGE_H_