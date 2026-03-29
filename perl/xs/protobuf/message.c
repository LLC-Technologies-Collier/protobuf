
#include "xs/protobuf/message.h"
#include "xs/protobuf.h"
#include "upb/reflection/def.h"
#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/arena.h"

SV *PerlUpb_WrapMessage(pTHX_ const upb_Message *msg, const upb_MessageDef *mdef, SV *arena_sv) {
    if (!msg) {
        return newSV(0); // Undef
    }

    SV* cached = PerlUpb_ObjCache_Get(aTHX_ msg);
    if (cached) return cached;

    HV *hv = newHV();
    SV *msg_sv = newSViv(PTR2IV(msg));
    hv_store(hv, "upb_msg", 7, msg_sv, 0);

    if (arena_sv) {
        hv_store(hv, "arena_sv", 8, newSVsv(arena_sv), 0);
    }
    // Store the descriptor C pointer for now
    SV *desc_sv = newSViv(PTR2IV(mdef));
    hv_store(hv, "_descriptor", 11, desc_sv, 0);

    SV *self = newRV_noinc((SV*)hv);
    const char *full_name = upb_MessageDef_FullName(mdef);
    char *class_name = NULL;
    
    // Convert dot to ::
    if (full_name) {
        size_t dot_count = 0;
        for (const char *p = full_name; *p; p++) if (*p == '.') dot_count++;
        class_name = (char*)safemalloc(strlen(full_name) + dot_count + 1);
        char *dst = class_name;
        for (const char *src = full_name; *src; src++) {
            if (*src == '.') {
                *dst++ = ':';
                *dst++ = ':';
            } else {
                *dst++ = *src;
            }
        }
        *dst = '\0';
    } else {
        class_name = savepv("Protobuf::Message");
    }

    sv_bless(self, gv_stashpv(class_name, GV_ADD));
    safefree(class_name);

    PerlUpb_ObjCache_Add(aTHX_ msg, self);

    return self;
}

SV* PerlUpb_MaybeGetMessage(pTHX_ const upb_Message *msg) {
    if (!msg) return NULL;
    return PerlUpb_ObjCache_Get(aTHX_ msg);
}

void PerlUpb_Message_Free(pTHX_ SV *message_sv) {
    const upb_Message *msg = PerlUpb_Message_GetMsg(aTHX_ message_sv);
    if (msg) {
        PerlUpb_ObjCache_Delete(aTHX_ msg);
        // The upb_Message is freed when the arena is freed.
        // We just clear the internal pointers.
        HV* hv = (HV*)SvRV(message_sv);
        hv_delete(hv, "upb_msg", 7, G_DISCARD);
    }
}


const upb_Message* PerlUpb_Message_GetMsg(pTHX_ SV* message_sv) {
    if (!message_sv || !SvROK(message_sv) || SvTYPE(SvRV(message_sv)) != SVt_PVHV) return NULL;
    HV* hv = (HV*)SvRV(message_sv);
    SV** svp = hv_fetch(hv, "upb_msg", 7, 0);
    return svp ? (const upb_Message*)SvIV(*svp) : NULL;
}

const upb_MessageDef* PerlUpb_Message_GetDef(pTHX_ SV* message_sv) {
    if (!message_sv || !SvROK(message_sv) || SvTYPE(SvRV(message_sv)) != SVt_PVHV) return NULL;
    HV* hv = (HV*)SvRV(message_sv);
    SV** svp = hv_fetch(hv, "_descriptor", 11, 0);
    return svp ? (const upb_MessageDef*)SvIV(*svp) : NULL;
}

SV* PerlUpb_Message_GetArena(pTHX_ SV* message_sv) {
    if (!message_sv || !SvROK(message_sv) || SvTYPE(SvRV(message_sv)) != SVt_PVHV) return NULL;
    HV* hv = (HV*)SvRV(message_sv);
    SV** svp = hv_fetch(hv, "arena_sv", 8, 0);
    return svp ? *svp : NULL;
}

