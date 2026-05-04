#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/registry.h"
#include "upb/mem/arena.h"
#include <unistd.h>
#include <time.h>

// -- Stats Tracking & Chaos Allocator --

#include "xs/protobuf/obj_cache.h"

#include <sys/mman.h>
#include <linux/mempolicy.h>
#include <sys/syscall.h>

// Helper to call mbind without libnuma
static long my_mbind(void *start, unsigned long len, int mode,
                     const unsigned long *nmask, unsigned long maxnode,
                     unsigned flags) {
    return syscall(SYS_mbind, start, len, mode, nmask, maxnode, flags);
}

static void* PerlUpb_StatsAlloc_Func(upb_alloc* alloc, void* ptr, size_t oldsize,
                                     size_t size, size_t* actual_size) {
    if (!alloc) return upb_alloc_global.func(&upb_alloc_global, ptr, oldsize, size, actual_size);
    PerlUpb_StatsAlloc* s = (PerlUpb_StatsAlloc*)alloc;

    if (s->poisoned) {
        return NULL;
    }

    dTHX;
    if (!aTHX) return upb_alloc_global.func(&upb_alloc_global, ptr, oldsize, size, actual_size);

    // ... (chaos logic)
    if (s->use_chaos && size > 0) {
        PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
        if (reg && reg->chaos.enabled) {
            // 1. Fail probability (prefers StatsAlloc override, fallbacks to Registry)
            double fail_p = (s->fail_probability > 0) ? s->fail_probability : reg->chaos.fail_probability;
            double r = (double)rand_r(&reg->chaos.seed) / (double)RAND_MAX;
            if (r < fail_p) {
                return NULL;
            }

            // 2. Delay probability
            double delay_p = (s->delay_probability > 0) ? s->delay_probability : reg->chaos.delay_probability;
            r = (double)rand_r(&reg->chaos.seed) / (double)RAND_MAX;
            if (r < delay_p) {
                uint32_t delay = rand_r(&reg->chaos.seed) % reg->chaos.max_delay_ms;
                usleep(delay * 1000);
            }
        }
    }

    void* ret = NULL;
    
    if (size > 0) {
        // Allocate/Realloc
        size_t requested_size = size + 2 * PERL_UPB_CANARY_SIZE;
        size_t old_requested_size = ptr ? (oldsize + 2 * PERL_UPB_CANARY_SIZE) : 0;
        void* old_ptr = ptr ? (char*)ptr - PERL_UPB_CANARY_SIZE : NULL;
        
        if (ptr) PerlUpb_VerifyCanaries(ptr, oldsize, "Before realloc", &s->poisoned);

        void* raw;
        if (s->numa_node != -1) {
            // Use mmap + mbind for NUMA affinity
            raw = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (raw != MAP_FAILED) {
                unsigned long mask = (1UL << s->numa_node);
                if (my_mbind(raw, requested_size, MPOL_BIND, &mask, sizeof(mask) * 8, 0) != 0) {
                    // Mbind failed, we still have the memory but policy isn't set.
                }
                if (old_ptr) {
                    memcpy(raw, old_ptr, old_requested_size < requested_size ? old_requested_size : requested_size);
                    munmap(old_ptr, old_requested_size);
                }
            } else {
                raw = NULL;
            }
        } else {
            raw = upb_alloc_global.func(&upb_alloc_global, old_ptr, old_requested_size, requested_size, NULL);
        }

        if (raw) {
            PerlUpb_WriteCanaries(raw, size);
            ret = (char*)raw + PERL_UPB_CANARY_SIZE;
            
            if (ptr == NULL) {
                s->total_reserved += size;
                s->total_blocks++;
                if (s->total_reserved > s->historical_max_size) {
                    s->historical_max_size = s->total_reserved;
                }
                PerlUpb_ObjCache_LogEvent(aTHX, ALLOC_EVENT_MALLOC, ret);
            } else {
                s->total_reserved = (s->total_reserved - oldsize) + size;
                if (s->total_reserved > s->historical_max_size) {
                    s->historical_max_size = s->total_reserved;
                }
                PerlUpb_ObjCache_LogEvent(aTHX, ALLOC_EVENT_REALLOC, ret);
            }
            if (actual_size) *actual_size = size;
        }
    } else if (ptr != NULL) {
        // Free
        PerlUpb_VerifyCanaries(ptr, oldsize, "Before free", &s->poisoned);
        void* raw = (char*)ptr - PERL_UPB_CANARY_SIZE;
        if (s->numa_node != -1) {
            munmap(raw, oldsize + 2 * PERL_UPB_CANARY_SIZE);
        } else {
            upb_alloc_global.func(&upb_alloc_global, raw, oldsize + 2 * PERL_UPB_CANARY_SIZE, 0, NULL);
        }
        s->total_reserved -= oldsize;
        s->total_blocks--;
        PerlUpb_ObjCache_LogEvent(aTHX, ALLOC_EVENT_FREE, ptr);
    }
    
    return ret;
}

// -- Arena Factory Implementation --

// -- Arena Factory Implementation --

void PerlUpb_Arena_Release(pTHX_ upb_Arena* arena, PerlUpb_ArenaLifecycle lifecycle) {
    if (arena) upb_Arena_Free(arena);
}

// Specialized Acquire for Stats Tracking
upb_Arena* PerlUpb_Arena_AcquireWithStats(pTHX_ PerlUpb_StatsAlloc* s) {
    if (!s) return upb_Arena_New();
    s->base.func = PerlUpb_StatsAlloc_Func;
    s->total_reserved = 0;
    s->total_blocks = 0;
    s->numa_node = -1;
    s->use_chaos = true;
    s->poisoned = false;
    s->fail_probability = 0;
    s->delay_probability = 0;

    size_t hint = s->historical_max_size > 0 ? s->historical_max_size : 0;
    if (hint > 1024 * 1024) hint = 1024 * 1024; // Cap at 1MB

    // CRITICAL: use upb_Arena_Init with hint
    upb_Arena* arena = upb_Arena_Init(NULL, hint, &s->base);
    if (!arena) {
        croak("Failed to acquire upb_Arena (StatsAlloc)");
    }
    return arena;
}

upb_Arena* PerlUpb_Arena_Acquire(pTHX_ PerlUpb_ArenaLifecycle lifecycle) {
    if (lifecycle == PERL_UPB_LIFECYCLE_TRANSIENT) {
        // FAST PATH: bypass stats tracking for transient arenas
        return upb_Arena_New();
    }

    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    if (reg) {
        return PerlUpb_Arena_AcquireWithStats(aTHX, &reg->stats_alloc);
    }
    return upb_Arena_New();
}

// -- Arena Wrapper Functions --

void* PerlUpb_Arena_CreateRaw(pTHX) {
    PerlUpb_Arena *arena_wrapper = (PerlUpb_Arena *)safemalloc(sizeof(PerlUpb_Arena));
    if (!arena_wrapper) {
        croak("Failed to allocate PerlUpb_Arena");
    }
    // For raw wrapper creation, we use its own local stats_alloc
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
            // We don't use Arena_Release here because we WANT it freed
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
    
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    HV* stash = (reg && reg->stash_arena) ? reg->stash_arena : gv_stashpv("Protobuf::Arena", GV_ADD);
    sv_bless(rv, stash);

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
    if (PL_dirty) return; // Let Perl handle cleanup during global destruction
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
