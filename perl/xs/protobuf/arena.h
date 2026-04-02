#ifndef PERL_PROTOBUF_ARENA_H_
#define PERL_PROTOBUF_ARENA_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/mem/arena.h"

// Arena wrappers
typedef struct {
    upb_Arena *arena;
} PerlUpb_Arena;

SV *PerlUpb_Arena_New(pTHX);
upb_Arena *PerlUpb_Arena_Get(pTHX_ SV *sv);
void PerlUpb_Arena_Free(pTHX_ SV *sv);
void PerlUpb_Arena_Destroy(pTHX_ SV *sv);

// Low-level XS helpers
void* PerlUpb_Arena_CreateRaw(pTHX);
void  PerlUpb_Arena_DestroyRaw(pTHX_ void* ptr);
upb_Arena* PerlUpb_Arena_GetRaw(pTHX_ void* ptr);

// Returns memory usage in bytes
uintptr_t PerlUpb_Arena_SpaceAllocated(pTHX_ SV *sv);

#endif // PERL_PROTOBUF_ARENA_H_
