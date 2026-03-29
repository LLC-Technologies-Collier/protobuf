#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/message/message.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "xs/descriptor/message.h"

SV* PerlUpb_Message_NewMessage(pTHX_ SV* descriptor_sv) {
    const upb_MessageDef *mdef = PerlUpb_MessageDef_GetMessage(aTHX_ descriptor_sv);
    if (!mdef) {
        croak("descriptor_sv must be a Protobuf::MessageDescriptor");
    }

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    const upb_MiniTable *mt = upb_MessageDef_MiniTable(mdef);
    if (!mt) {
        SvREFCNT_dec(arena_sv);
        croak("Failed to get MiniTable for message");
    }

    upb_Message *msg = upb_Message_New(mt, arena);
    if (!msg) {
        SvREFCNT_dec(arena_sv);
        croak("Failed to allocate upb_Message");
    }

    SV *msg_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
    SvREFCNT_dec(arena_sv); // PerlUpb_WrapMessage stores a copy (refcnt inc'd or similar, wait)

    return msg_sv;
}
