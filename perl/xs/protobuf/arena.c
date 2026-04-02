#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/arena.h"
#include "perl/xs/protobuf/registry.h"
#include "upb/mem/arena.h"

// -- Canary Logic --

// (Now in arena.h as static inline)

// -- Stats Tracking Allocator --

static void* PerlUpb_StatsAlloc_Func(upb_alloc* alloc, void* ptr, size_t oldsize,
                                     size_t size, size_t* actual_size) {
    PerlUpb_StatsAlloc* s = (PerlUpb_StatsAlloc*)alloc;
    void* ret = NULL;
    
    // For now, assume canaries are ALWAYS enabled for testing or if requested.
    // We can pull a flag from the registry later.
    
    if (size > 0) {
        // Allocate/Realloc
        size_t requested_size = size + 2 * PERL_UPB_CANARY_SIZE;
        size_t old_requested_size = ptr ? (oldsize + 2 * PERL_UPB_CANARY_SIZE) : 0;
        void* old_ptr = ptr ? (char*)ptr - PERL_UPB_CANARY_SIZE : NULL;
        
        if (ptr) PerlUpb_VerifyCanaries(ptr, oldsize, "Before realloc");

        void* raw = upb_alloc_global.func(&upb_alloc_global, old_ptr, old_requested_size, requested_size, NULL);
        if (raw) {
            PerlUpb_WriteCanaries(raw, size);
            ret = (char*)raw + PERL_UPB_CANARY_SIZE;
            
            if (ptr == NULL) {
                s->total_reserved += size;
                s->total_blocks++;
            } else {
                s->total_reserved = (s->total_reserved - oldsize) + size;
            }
        }
    } else if (ptr != NULL) {
        // Free
        PerlUpb_VerifyCanaries(ptr, oldsize, "Before free");
        void* raw = (char*)ptr - PERL_UPB_CANARY_SIZE;
        upb_alloc_global.func(&upb_alloc_global, raw, oldsize + 2 * PERL_UPB_CANARY_SIZE, 0, NULL);
        s->total_reserved -= oldsize;
        s->total_blocks--;
    }
    
    if (actual_size && ret) *actual_size = size;
    return ret;
}

// -- Arena Factory Implementation --

upb_Arena* PerlUpb_Arena_Acquire(pTHX_ PerlUpb_ArenaLifecycle lifecycle) {
    if (lifecycle == PERL_UPB_LIFECYCLE_TRANSIENT) {
        PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
        if (reg->cached_transient_arena) {
            // Check for excessive growth (e.g. > 1MB) to prevent leakage
            // but for now we just free and recreate since Reset is missing.
            // In a future version of upb, we'll use a real reset.
            PerlUpb_Arena_Release(aTHX_ reg->cached_transient_arena, PERL_UPB_LIFECYCLE_TRANSIENT);
        }
        reg->cached_transient_arena = upb_Arena_New();
        return reg->cached_transient_arena;
    }
    return upb_Arena_New();
}

void PerlUpb_Arena_Release(pTHX_ upb_Arena* arena, PerlUpb_ArenaLifecycle lifecycle) {
    if (lifecycle == PERL_UPB_LIFECYCLE_TRANSIENT) {
        // Cached arenas are kept alive until interpreter destruction
        return;
    }
    if (arena) upb_Arena_Free(arena);
}

// Specialized Acquire for Stats Tracking
upb_Arena* PerlUpb_Arena_AcquireWithStats(pTHX_ PerlUpb_StatsAlloc* s) {
    s->base.func = PerlUpb_StatsAlloc_Func;
    s->total_reserved = 0;
    s->total_blocks = 0;
    return upb_Arena_Init(NULL, 0, &s->base);
}

// -- Arena Wrapper Functions --

void* PerlUpb_Arena_CreateRaw(pTHX) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)safemalloc(sizeof(PerlUpb_Arena));
    if (!arena_wrapper) {
        croak("Failed to allocate PerlUpb_Arena");
    }
    arena_wrapper->arena = PerlUpb_Arena_AcquireWithStats(aTHX_ &arena_wrapper->stats_alloc);
    if (!arena_wrapper->arena) {
        safefree(arena_wrapper);
        croak("Failed to acquire upb_Arena");
    }
    return (void*)arena_wrapper;
}

void PerlUpb_Arena_DestroyRaw(pTHX_ void* ptr) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)ptr;
    if (arena_wrapper) {
        if (arena_wrapper->arena) {
            PerlUpb_Arena_Release(aTHX_ arena_wrapper->arena, PERL_UPB_LIFECYCLE_PERMANENT);
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
    return (uintptr_t)upb_Arena_SpaceAllocated(arena, NULL);
}

uintptr_t PerlUpb_Arena_SpaceReserved(pTHX_ SV *sv) {
    PerlUpb_ArenaStats stats;
    PerlUpb_Arena_GetStats(aTHX, sv, &stats);
    return (uintptr_t)stats.reserved;
}

void PerlUpb_Arena_GetStats(pTHX_ SV *sv, PerlUpb_ArenaStats *stats) {
    if (!sv || !SvROK(sv) || !sv_isa(sv, "Protobuf::Arena")) {
        memset(stats, 0, sizeof(PerlUpb_ArenaStats));
        return;
    }
    
    SV *rv = SvRV(sv);
    void* raw_ptr = NULL;
    bool is_tmpfs = false;

    if (SvTYPE(rv) == SVt_PVHV) {
        SV** is_tmpfs_p = hv_fetch((HV*)rv, "_is_tmpfs", 9, 0);
        is_tmpfs = (is_tmpfs_p && SvTRUE(*is_tmpfs_p));

        SV** svp = hv_fetch((HV*)rv, "_arena_ptr", 10, 0);
        if (svp && SvIOK(*svp)) {
            raw_ptr = INT2PTR(void*, SvIV(*svp));
        }
    }

    if (!raw_ptr) {
        memset(stats, 0, sizeof(PerlUpb_ArenaStats));
        return;
    }

    if (is_tmpfs) {
        PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)raw_ptr;
        stats->allocated = wrapper->alloc->offset;
        stats->reserved = wrapper->alloc->size;
        stats->blocks = 1;
    } else {
        PerlUpb_Arena* wrapper = (PerlUpb_Arena*)raw_ptr;
        stats->allocated = upb_Arena_SpaceAllocated(wrapper->arena, NULL);
        stats->reserved = wrapper->stats_alloc.total_reserved;
        stats->blocks = wrapper->stats_alloc.total_blocks;
    }
}
