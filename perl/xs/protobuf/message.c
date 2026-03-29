
#include "xs/protobuf/message.h"
#include "xs/protobuf.h"
#include "upb/reflection/def.h"

SV *PerlUpb_WrapMessage(pTHX_ const upb_Message *msg, const upb_MessageDef *mdef, SV *arena_sv) {
    // TODO: Full implementation in Milestone 16
    if (!msg) {
        return newSV(0); // Undef
    }

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
    const char *class_name = upb_MessageDef_FullName(mdef);
    if (!class_name) {
        class_name = "Protobuf::Message"; // Fallback
    }
    sv_bless(self, gv_stashpv(class_name, GV_ADD));
    return self;
}

SV* PerlUpb_MaybeGetMessage(pTHX_ const upb_Message *msg) {
    (void)msg;
    return NULL; // Simulate cache miss
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

