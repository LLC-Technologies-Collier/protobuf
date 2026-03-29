#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/message/serialize.h"
#include "xs/protobuf/message.h"
#include "xs/protobuf/arena.h"
#include "xs/descriptor/message.h"
#include "upb/wire/decode.h"
#include "upb/wire/encode.h"
#include "upb/reflection/def.h"

SV* PerlUpb_Message_Parse(pTHX_ SV* descriptor_sv, SV* data_sv) {
    const upb_MessageDef *mdef = PerlUpb_MessageDef_GetMessage(aTHX_ descriptor_sv);
    if (!mdef) {
        croak("descriptor_sv must be a Protobuf::MessageDescriptor");
    }

    STRLEN len;
    const char* data = SvPV(data_sv, len);

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

    // TODO: Support extension registries
    upb_DecodeStatus status = upb_Decode(data, len, msg, mt, NULL, 0, arena);
    if (status != kUpb_DecodeStatus_Ok) {
        SvREFCNT_dec(arena_sv);
        croak("Failed to parse message: %d", status);
    }

    SV *msg_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
    SvREFCNT_dec(arena_sv);

    return msg_sv;
}

SV* PerlUpb_Message_Serialize(pTHX_ SV* message_sv) {
    const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ message_sv);
    const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ message_sv);
    if (!msg || !mdef) {
        croak("Invalid message object");
    }

    const upb_MiniTable *mt = upb_MessageDef_MiniTable(mdef);
    if (!mt) croak("Failed to get MiniTable");

    upb_Arena* enc_arena = upb_Arena_New();
    char* buf = NULL;
    size_t size = 0;

    upb_EncodeStatus status = upb_Encode(msg, mt, 0, enc_arena, &buf, &size);
    if (status != kUpb_EncodeStatus_Ok) {
        upb_Arena_Free(enc_arena);
        croak("Failed to serialize message: %d", status);
    }

    SV* result = newSVpvn(buf, size);
    upb_Arena_Free(enc_arena);
    return result;
}
