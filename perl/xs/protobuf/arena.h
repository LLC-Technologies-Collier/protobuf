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

#endif // PERL_PROTOBUF_ARENA_H_
