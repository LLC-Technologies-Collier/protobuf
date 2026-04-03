
#include "xs/protobuf/message.h"
#include "xs/protobuf.h"
#include "upb/reflection/def.h"
#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/utils.h"

static int descriptor_cleanup(pTHX_ SV* sv, MAGIC* mg) {
    // No explicit cleanup needed for the descriptor IV,
    // but having the magic ensures we don't trigger the "unreferenced scalar"
    // warnings during global destruction by explicitly managing it.
    return 0;
}

static MGVTBL descriptor_vtbl = {
    NULL, NULL, NULL, NULL, descriptor_cleanup
};

SV *PerlUpb_WrapMessage(pTHX_ const upb_Message *msg, const upb_MessageDef *mdef, SV *arena_sv) {
    if (!msg) {
        return newSV(0); // Undef
    }

    SV* cached = PerlUpb_ObjCache_Get(aTHX_ msg);
    if (cached) return cached;

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

    SV *self = PerlUpb_WrapArenaBoundObject(aTHX_ msg, arena_sv, class_name);
    safefree(class_name);

    // Store the descriptor C pointer in the HV.
    // We don't use magic here anymore to reduce "unreferenced scalar" noise.
    HV* hv = (HV*)SvRV(self);
    hv_store(hv, "_descriptor", 11, newSViv(PTR2IV(mdef)), 0);

    return self;
}

SV* PerlUpb_MaybeGetMessage(pTHX_ const upb_Message *msg) {
    if (!msg) return NULL;
    return PerlUpb_ObjCache_Get(aTHX_ msg);
}

void PerlUpb_Message_Free(pTHX_ SV *message_sv) {
    if (PL_dirty) return; // Let Perl handle cleanup during global destruction

    // We don't call PerlUpb_ObjCache_Delete here because it's handled
    // by the magic wrapper_cleanup in utils.c. Calling it here causes
    // double-deletion and "unreferenced scalar" warnings during eval cleanup.
    
    // The upb_Message is freed when the arena is freed.
    // We just clear the internal pointers in the Perl object.
    HV* hv = (HV*)SvRV(message_sv);
    if (hv_exists(hv, "_upb_ptr", 8)) {
        hv_delete(hv, "_upb_ptr", 8, G_DISCARD);
    }
}


const upb_Message* PerlUpb_Message_GetMsg(pTHX_ SV* message_sv) {
    if (!message_sv || !SvROK(message_sv) || SvTYPE(SvRV(message_sv)) != SVt_PVHV) return NULL;
    HV* hv = (HV*)SvRV(message_sv);
    SV** svp = hv_fetch(hv, "_upb_ptr", 8, 0);
    return svp ? (const upb_Message*)SvIV(*svp) : NULL;
}

const upb_MessageDef* PerlUpb_Message_GetDef(pTHX_ SV* message_sv) {
    if (!message_sv || !SvROK(message_sv) || SvTYPE(SvRV(message_sv)) != SVt_PVHV) return NULL;
    HV* hv = (HV*)SvRV(message_sv);
    SV** svp = hv_fetch(hv, "_descriptor", 11, 0);
    return svp ? (const upb_MessageDef*)SvIV(*svp) : NULL;
}

SV* PerlUpb_Message_GetArena(pTHX_ SV* message_sv) {
    return PerlUpb_GetArenaFromObject(aTHX_ message_sv);
}

SV* PerlUpb_Message_GetFingerprint(pTHX_ SV* message_sv) {
    SV* arena_sv = PerlUpb_Message_GetArena(aTHX_ message_sv);
    const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ message_sv);
    
    if (!msg) return &PL_sv_undef;

    if (arena_sv && PerlUpb_Arena_IsTmpfs(aTHX_ arena_sv)) {
        const char* path = PerlUpb_Arena_GetPath(aTHX_ arena_sv);
        size_t offset = PerlUpb_Arena_GetOffset(aTHX_ arena_sv, (void*)msg);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s:%zu", path ? path : "anon", offset);
        return newSVpv(buf, 0);
    } else {
        // Fallback for standard arenas: just use memory address as local fingerprint
        char buf[32];
        snprintf(buf, sizeof(buf), "mem:%p", msg);
        return newSVpv(buf, 0);
    }
}

