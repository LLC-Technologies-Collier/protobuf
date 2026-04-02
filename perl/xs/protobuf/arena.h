#ifndef PERL_PROTOBUF_ARENA_H_
#define PERL_PROTOBUF_ARENA_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/mem/arena.h"

#include "upb/mem/alloc.h"

// Stats-tracking allocator
typedef struct {
    upb_alloc base;
    size_t total_reserved;
    size_t total_blocks;
    bool use_chaos;
} PerlUpb_StatsAlloc;

// Arena wrappers
typedef struct {
    upb_Arena *arena;
    PerlUpb_StatsAlloc stats_alloc;
} PerlUpb_Arena;

typedef enum {
    PERL_UPB_BLOCK_MMAP = 0,
    PERL_UPB_BLOCK_MALLOC = 1
} PerlUpb_BlockType;

typedef struct {
    upb_alloc base;
    PerlUpb_BlockType type;
    int fd;         // Used if MMAP
    void* region;
    size_t size;
    size_t offset;
} PerlUpb_BlockAlloc;

// Special wrapper for Custom Allocator Arenas (Tmpfs / Block)
typedef struct {
    PerlUpb_Arena base;
    PerlUpb_BlockAlloc* alloc;
} PerlUpb_Arena_Custom;

// Arena Lifecycle Hints
typedef enum {
    PERL_UPB_LIFECYCLE_PERMANENT = 0, // Stays in Perl (e.g. Message, Pool)
    PERL_UPB_LIFECYCLE_TRANSIENT = 1  // Short-lived (e.g. transient serialization)
} PerlUpb_ArenaLifecycle;

// Arena Factory
upb_Arena* PerlUpb_Arena_Acquire(pTHX_ PerlUpb_ArenaLifecycle lifecycle);
void       PerlUpb_Arena_Release(pTHX_ upb_Arena* arena, PerlUpb_ArenaLifecycle lifecycle);

SV *PerlUpb_Arena_New(pTHX);
SV *PerlUpb_Arena_NewTmpfs(pTHX_ const char* path, size_t size);
SV *PerlUpb_Arena_AttachTmpfs(pTHX_ const char* path, size_t size);
upb_Arena *PerlUpb_Arena_Get(pTHX_ SV *sv);
void PerlUpb_Arena_Free(pTHX_ SV *sv);
void PerlUpb_Arena_Destroy(pTHX_ SV *sv);

// Low-level XS helpers
void* PerlUpb_Arena_CreateRaw(pTHX);
void  PerlUpb_Arena_DestroyRaw(pTHX_ void* ptr);
void  PerlUpb_Arena_DestroyRaw_Tmpfs(pTHX_ void* ptr, bool is_tmpfs);
upb_Arena* PerlUpb_Arena_GetRaw(pTHX_ void* ptr);

// Returns memory usage in bytes
uintptr_t PerlUpb_Arena_SpaceAllocated(pTHX_ SV *sv);
uintptr_t PerlUpb_Arena_SpaceReserved(pTHX_ SV *sv);

typedef struct {
    size_t allocated;
    size_t reserved;
    size_t blocks;
} PerlUpb_ArenaStats;

void PerlUpb_Arena_GetStats(pTHX_ SV *sv, PerlUpb_ArenaStats *stats);

// -- Canary Logic --
#define PERL_UPB_CANARY_SIZE 16
#define PERL_UPB_CANARY_PATTERN 0xDEADBEEFCAFEBABEULL

static inline void PerlUpb_WriteCanaries(void* ptr, size_t size) {
    uint64_t* start = (uint64_t*)ptr;
    uint64_t* end = (uint64_t*)((char*)ptr + PERL_UPB_CANARY_SIZE + size);
    start[0] = PERL_UPB_CANARY_PATTERN;
    start[1] = PERL_UPB_CANARY_PATTERN;
    end[0] = PERL_UPB_CANARY_PATTERN;
    end[1] = PERL_UPB_CANARY_PATTERN;
}

static inline void PerlUpb_VerifyCanaries(void* ptr, size_t size, const char* msg) {
    uint64_t* start = (uint64_t*)((char*)ptr - PERL_UPB_CANARY_SIZE);
    uint64_t* end = (uint64_t*)((char*)ptr + size);
    if (start[0] != PERL_UPB_CANARY_PATTERN || start[1] != PERL_UPB_CANARY_PATTERN) {
        croak("MEMORY CORRUPTION DETECTED (Underflow): %s", msg);
    }
    if (end[0] != PERL_UPB_CANARY_PATTERN || end[1] != PERL_UPB_CANARY_PATTERN) {
        croak("MEMORY CORRUPTION DETECTED (Overflow): %s", msg);
    }
}

#endif // PERL_PROTOBUF_ARENA_H_
