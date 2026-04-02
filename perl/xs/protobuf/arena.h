#ifndef PERL_PROTOBUF_ARENA_H_
#define PERL_PROTOBUF_ARENA_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/mem/arena.h"

// Arena wrappers
typedef struct {
    upb_Arena *arena;
} PerlUpb_Arena;

// Arena Lifecycle Hints
typedef enum {
    PERL_UPB_LIFECYCLE_PERMANENT = 0, // Stays in Perl (e.g. Message, Pool)
    PERL_UPB_LIFECYCLE_TRANSIENT = 1  // Short-lived (e.g. transient serialization)
} PerlUpb_ArenaLifecycle;

// Arena Factory
upb_Arena* PerlUpb_Arena_Acquire(pTHX_ PerlUpb_ArenaLifecycle lifecycle);

SV *PerlUpb_Arena_New(pTHX);
SV *PerlUpb_Arena_NewTmpfs(pTHX_ const char* path, size_t size);
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

#endif // PERL_PROTOBUF_ARENA_H_
