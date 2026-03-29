#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/unknown_fields/set.h"
#include "xs/protobuf/message.h"
#include "xs/protobuf/arena.h"
#include "upb/message/message.h"
#include "upb/message/internal/message.h"

// MUST be last.
#include "upb/port/def.inc"

typedef struct {
    SV* message_sv;
} PerlUpb_UnknownFieldSet;

SV* PerlUpb_UnknownFieldSet_New(pTHX_ SV* message_sv) {
    PerlUpb_UnknownFieldSet* s = (PerlUpb_UnknownFieldSet*)malloc(sizeof(PerlUpb_UnknownFieldSet));
    s->message_sv = newSVsv(message_sv);

    SV* sv = newSViv((IV)s);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::UnknownFieldSet", GV_ADD));
    return obj;
}

static PerlUpb_UnknownFieldSet* GetSet(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::UnknownFieldSet")) {
        return NULL;
    }
    return (PerlUpb_UnknownFieldSet*)SvIV(SvRV(sv));
}

SV* PerlUpb_UnknownFieldSet_GetData(pTHX_ SV* self) {
    PerlUpb_UnknownFieldSet* s = GetSet(aTHX_ self);
    if (!s) return &PL_sv_undef;

    const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ s->message_sv);
    if (!msg) return &PL_sv_undef;

    uintptr_t iter = kUpb_Message_UnknownBegin;
    upb_StringView data;
    
    // First pass to calculate total length
    size_t total_len = 0;
    while (upb_Message_NextUnknown(msg, &data, &iter)) {
        total_len += data.size;
    }

    if (total_len == 0) return newSVpv("", 0);

    SV* result = newSV(total_len + 1);
    char* buf = SvPVX(result);
    size_t offset = 0;

    iter = kUpb_Message_UnknownBegin;
    while (upb_Message_NextUnknown(msg, &data, &iter)) {
        memcpy(buf + offset, data.data, data.size);
        offset += data.size;
    }
    buf[total_len] = '\0';
    SvCUR_set(result, total_len);
    SvPOK_on(result);

    return result;
}

void PerlUpb_UnknownFieldSet_Add(pTHX_ SV* self, SV* data_sv) {
    PerlUpb_UnknownFieldSet* s = GetSet(aTHX_ self);
    if (!s) return;

    upb_Message* msg = (upb_Message*)PerlUpb_Message_GetMsg(aTHX_ s->message_sv);
    if (!msg) return;

    STRLEN len;
    const char* data = SvPV(data_sv, len);

    SV* arena_sv = PerlUpb_Message_GetArena(aTHX_ s->message_sv);
    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    UPB_PRIVATE(_upb_Message_AddUnknown)(msg, data, len, arena, kUpb_AddUnknown_Copy);
}

void PerlUpb_UnknownFieldSet_Clear(pTHX_ SV* self) {
    PerlUpb_UnknownFieldSet* s = GetSet(aTHX_ self);
    if (!s) return;

    upb_Message* msg = (upb_Message*)PerlUpb_Message_GetMsg(aTHX_ s->message_sv);
    if (!msg) return;

    _upb_Message_DiscardUnknown_shallow(msg);
}

void PerlUpb_UnknownFieldSet_Free(pTHX_ SV* sv) {
    PerlUpb_UnknownFieldSet* s = GetSet(aTHX_ sv);
    if (s) {
        SvREFCNT_dec(s->message_sv);
        free(s);
        sv_setiv(SvRV(sv), 0);
    }
}
