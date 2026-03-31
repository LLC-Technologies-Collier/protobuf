#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/message/serialize.h"
#include "xs/protobuf/message.h"
#include "xs/protobuf/arena.h"
#include "xs/descriptor/message.h"
#include "xs/descriptor_pool/pool.h"
#include "upb/wire/decode.h"
#include "upb/wire/encode.h"
#include "upb/text/encode.h"
#include "upb/json/encode.h"
#include "upb/json/decode.h"
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

SV* PerlUpb_Message_ToText(pTHX_ SV* message_sv) {
    const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ message_sv);
    const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ message_sv);
    if (!msg || !mdef) {
        croak("Invalid message object");
    }

    size_t size = upb_TextEncode(msg, mdef, NULL, 0, NULL, 0);
    
    char* buf = (char*)malloc(size + 1);
    if (!buf) croak("Out of memory encoding text format");
    
    size_t encoded = upb_TextEncode(msg, mdef, NULL, 0, buf, size + 1);
    
    SV* result = newSVpvn(buf, encoded);
    free(buf);
    return result;
}

SV* PerlUpb_Message_ToJson(pTHX_ SV* message_sv) {
    const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ message_sv);
    const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ message_sv);
    if (!msg || !mdef) croak("Invalid message object");

    const upb_DefPool* ext_pool = upb_FileDef_Pool(upb_MessageDef_File(mdef));
    upb_Status status;
    upb_Status_Clear(&status);

    size_t size = upb_JsonEncode(msg, mdef, ext_pool, 0, NULL, 0, &status);
    if (!upb_Status_IsOk(&status)) {
        croak("JSON Encode error: %s", upb_Status_ErrorMessage(&status));
    }
    
    char* buf = (char*)malloc(size + 1);
    if (!buf) croak("Out of memory encoding JSON");
    
    size_t encoded = upb_JsonEncode(msg, mdef, ext_pool, 0, buf, size + 1, &status);
    if (!upb_Status_IsOk(&status)) {
        free(buf);
        croak("JSON Encode error: %s", upb_Status_ErrorMessage(&status));
    }
    
    SV* result = newSVpvn(buf, encoded);
    free(buf);
    return result;
}

SV* PerlUpb_Message_FromJson(pTHX_ SV* class_name, SV* json_sv) {
    STRLEN len;
    const char* json_str = SvPV(json_sv, len);
    const char* class_str = SvPV_nolen(class_name);

    char* full_name = savepv(class_str);
    for (char* p = full_name; *p; p++) {
        if (*p == ':' && *(p+1) == ':') {
            *p = '.';
            memmove(p+1, p+2, strlen(p+2) + 1);
        }
    }

    SV* pool_sv = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
    const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ pool_sv);
    const upb_MessageDef* mdef = upb_DefPool_FindMessageByName(pool, full_name);
    Safefree(full_name);

    if (!mdef) {
        croak("Could not find descriptor for message class %s", class_str);
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

    const upb_DefPool* ext_pool = upb_FileDef_Pool(upb_MessageDef_File(mdef));
    upb_Status status;
    upb_Status_Clear(&status);

    bool ok = upb_JsonDecode(json_str, len, msg, mdef, ext_pool, 0, arena, &status);
    if (!ok) {
        SvREFCNT_dec(arena_sv);
        croak("Failed to parse JSON: %s", upb_Status_ErrorMessage(&status));
    }

    SV *msg_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
    SvREFCNT_dec(arena_sv);

    return msg_sv;
}
