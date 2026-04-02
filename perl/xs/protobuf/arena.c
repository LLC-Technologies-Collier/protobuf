#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/arena.h"
#include "upb/mem/arena.h"

// -- Arena Wrapper Functions --

void* PerlUpb_Arena_CreateRaw(pTHX) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)safemalloc(sizeof(PerlUpb_Arena));
    if (!arena_wrapper) {
        croak("Failed to allocate PerlUpb_Arena");
    }
    arena_wrapper->arena = upb_Arena_New();
    if (!arena_wrapper->arena) {
        safefree(arena_wrapper);
        croak("Failed to create upb_Arena");
    }
    return (void*)arena_wrapper;
}

void PerlUpb_Arena_DestroyRaw(pTHX_ void* ptr) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)ptr;
    if (arena_wrapper) {
        if (arena_wrapper->arena) {
            upb_Arena_Free(arena_wrapper->arena);
        }
        safefree(arena_wrapper);
    }
}

upb_Arena* PerlUpb_Arena_GetRaw(pTHX_ void* ptr) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)ptr;
    return arena_wrapper ? arena_wrapper->arena : NULL;
}

// Create a new PerlUpb_Arena wrapper (Hash-based)
SV *PerlUpb_Arena_New(pTHX) {
    void* raw = PerlUpb_Arena_CreateRaw(aTHX);
    HV* hv = newHV();
    
    SV* ptr_sv = newSViv(PTR2IV(raw));
    hv_store(hv, "_arena_ptr", 10, ptr_sv, 0);

    SV *rv = newRV_noinc((SV*)hv);
    sv_bless(rv, gv_stashpv("Protobuf::Arena", GV_ADD));
    return rv;
}

// Get the raw upb_Arena * from a Protobuf::Arena SV
upb_Arena *PerlUpb_Arena_Get(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        croak("Argument is not a blessed Protobuf::Arena object");
    }
    
    SV* rv = SvRV(sv);
    PerlUpb_Arena *arena_wrapper = NULL;

    if (SvTYPE(rv) == SVt_PVHV) {
        // Hash-based object
        SV** svp = hv_fetch((HV*)rv, "_arena_ptr", 10, 0);
        if (svp && SvIOK(*svp)) {
            arena_wrapper = INT2PTR(PerlUpb_Arena *, SvIV(*svp));
        }
    } else if (SvIOK(rv)) {
        // IV-based object (Legacy/Low-level)
        arena_wrapper = INT2PTR(PerlUpb_Arena *, SvIV(rv));
    }

    if (!arena_wrapper) {
        croak("Invalid Protobuf::Arena object: pointer is NULL");
    }
    return arena_wrapper->arena;
}

// Free the arena
void PerlUpb_Arena_Free(pTHX_ SV *sv) {
    PerlUpb_Arena_Destroy(aTHX_ sv);
}

// Called from Protobuf::Arena::DEMOLISH or DESTROY
void PerlUpb_Arena_Destroy(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        return;
    }
    SV *rv = SvRV(sv);
    void* raw_ptr = NULL;
    bool is_tmpfs = false;

    if (SvTYPE(rv) == SVt_PVHV) {
        SV** is_tmpfs_p = hv_fetch((HV*)rv, "_is_tmpfs", 9, 0);
        if (is_tmpfs_p && SvTRUE(*is_tmpfs_p)) {
            is_tmpfs = true;
        }

        SV** svp = hv_fetch((HV*)rv, "_arena_ptr", 10, 0);
        if (svp && SvIOK(*svp)) {
            raw_ptr = INT2PTR(void*, SvIV(*svp));
            sv_setiv(*svp, 0); // Clear key in hash
        }
    } else if (SvIOK(rv)) {
        raw_ptr = INT2PTR(void*, SvIV(rv));
        sv_setiv(rv, 0); // Clear IV
    }

    if (raw_ptr) {
        PerlUpb_Arena_DestroyRaw_Tmpfs(aTHX_ raw_ptr, is_tmpfs);
    }
}

uintptr_t PerlUpb_Arena_SpaceAllocated(pTHX_ SV *sv) {
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ sv);
    return upb_Arena_SpaceAllocated(arena, NULL);
}
