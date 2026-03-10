#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/arena.h"
#include "upb/mem/arena.h"

// -- Arena Wrapper Functions --

// Create a new PerlUpb_Arena wrapper
SV *PerlUpb_Arena_New(pTHX) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)safemalloc(sizeof(PerlUpb_Arena));
    if (!arena_wrapper) {
        croak("Failed to allocate PerlUpb_Arena");
    }
    arena_wrapper->arena = upb_Arena_New();
    if (!arena_wrapper->arena) {
        safefree(arena_wrapper);
        croak("Failed to create upb_Arena");
    }
    SV *self = newSV(0);
    sv_setiv(self, PTR2IV(arena_wrapper)); // Store pointer as IV
    SV *rv = newRV_noinc(self);
    sv_bless(rv, gv_stashpv("Protobuf::Arena", GV_ADD));
    return rv;
}

// Get the raw upb_Arena * from a Protobuf::Arena SV
upb_Arena *PerlUpb_Arena_Get(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        croak("Argument is not a blessed Protobuf::Arena object");
    }
    SV *self = SvRV(sv);
    if (!SvIOK(self)) {
        croak("Protobuf::Arena object internal is not an IV");
    }
    IV tmp = SvIV(self);
    PerlUpb_Arena *arena_wrapper = INT2PTR(PerlUpb_Arena *, tmp);
    if (!arena_wrapper) {
        croak("Invalid Protobuf::Arena object: wrapper is NULL");
    }
    return arena_wrapper->arena;
}

// Free the arena
void PerlUpb_Arena_Free(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        return;
    }
    SV *self = SvRV(sv);
    if (!SvIOK(self)) return;
    IV tmp = SvIV(self);
    PerlUpb_Arena *arena_wrapper = INT2PTR(PerlUpb_Arena *, tmp);
    if (arena_wrapper && arena_wrapper->arena) {
        upb_Arena_Free(arena_wrapper->arena);
        arena_wrapper->arena = NULL;
    }
    // The arena_wrapper itself will be freed when the SV is destroyed
}

// Called from Protobuf::Arena::DEMOLISH
void PerlUpb_Arena_Destroy(pTHX_ SV *sv) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        return;
    }
    SV *self = SvRV(sv);
    if (!SvIOK(self)) return; // Should not happen
    IV tmp = SvIV(self);
    PerlUpb_Arena *arena_wrapper = INT2PTR(PerlUpb_Arena *, tmp);
    if (arena_wrapper) {
        if (arena_wrapper->arena) {
            upb_Arena_Free(arena_wrapper->arena);
        }
        safefree(arena_wrapper);
        sv_setiv(self, 0); // Clear pointer
    }
}
