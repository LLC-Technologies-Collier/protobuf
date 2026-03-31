#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/repeated/composite.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "upb/message/array.h"
#include "upb/reflection/def.h"

// Internal struct from repeated.c (should probably move to a private header if needed,
// but for now I'll just re-declare or use an accessor if I added one).
// Let's add accessors to repeated.h for internal use.
typedef struct {
    upb_Array* arr;
    const upb_FieldDef* f;
    SV* arena_sv;
} PerlUpb_Repeated;

static PerlUpb_Repeated* GetRepeated(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Internal::Repeated")) {
        return NULL;
    }
    return (PerlUpb_Repeated*)SvIV(SvRV(sv));
}

SV* PerlUpb_Repeated_Add(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return &PL_sv_undef;

    if (!upb_FieldDef_IsSubMessage(r->f)) {
        croak("add() can only be called on repeated message fields");
    }

    const upb_MessageDef* mdef = upb_FieldDef_MessageSubDef(r->f);
    const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ r->arena_sv);

    upb_Message* msg = upb_Message_New(mt, arena);
    if (!msg) croak("Failed to create new submessage");

    upb_MessageValue val;
    val.msg_val = msg;
    if (!upb_Array_Append(r->arr, val, arena)) {
        croak("Failed to append new message to array");
    }

    return PerlUpb_WrapMessage(aTHX_ msg, mdef, r->arena_sv);
}
