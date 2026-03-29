#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/repeated/repeated.h"
#include "xs/protobuf/arena.h"
#include "xs/convert.h"
#include "upb/message/array.h"
#include "upb/reflection/def.h"

typedef struct {
    upb_Array* arr;
    const upb_FieldDef* f;
    SV* arena_sv;
} PerlUpb_Repeated;

SV* PerlUpb_Repeated_New(pTHX_ upb_Array* arr, const upb_FieldDef* f, SV* arena_sv) {
    PerlUpb_Repeated* r = (PerlUpb_Repeated*)malloc(sizeof(PerlUpb_Repeated));
    r->arr = arr;
    r->f = f;
    r->arena_sv = newSVsv(arena_sv);

    SV* sv = newSViv((IV)r);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::Repeated", GV_ADD));
    return obj;
}

static PerlUpb_Repeated* GetRepeated(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Repeated")) {
        return NULL;
    }
    return (PerlUpb_Repeated*)SvIV(SvRV(sv));
}

SV* PerlUpb_Repeated_GetItem(pTHX_ SV* self, int index) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return &PL_sv_undef;

    size_t size = upb_Array_Size(r->arr);
    if (index < 0 || (size_t)index >= size) {
        return &PL_sv_undef;
    }

    upb_MessageValue val = upb_Array_Get(r->arr, index);
    return PerlUpb_UpbToSv_Element(aTHX_ &val, r->f, r->arena_sv);
}

void PerlUpb_Repeated_SetItem(pTHX_ SV* self, int index, SV* val_sv) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return;

    size_t size = upb_Array_Size(r->arr);
    if (index < 0 || (size_t)index >= size) {
        croak("Index out of bounds for repeated field");
    }

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ r->arena_sv);
    upb_MessageValue val;
    if (!PerlUpb_SvToUpb_Element(aTHX_ val_sv, r->f, &val, arena)) {
        croak("Failed to convert value for repeated field");
    }

    upb_Array_Set(r->arr, index, val);
}

void PerlUpb_Repeated_Append(pTHX_ SV* self, SV* val_sv) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return;

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ r->arena_sv);
    upb_MessageValue val;
    if (!PerlUpb_SvToUpb_Element(aTHX_ val_sv, r->f, &val, arena)) {
        croak("Failed to convert value for repeated field");
    }

    if (!upb_Array_Append(r->arr, val, arena)) {
        croak("Failed to append to repeated field");
    }
}

void PerlUpb_Repeated_Delete(pTHX_ SV* self, int index, int count) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return;

    size_t size = upb_Array_Size(r->arr);
    if (index < 0 || (size_t)index > size) return;
    if (index + count > (int)size) count = size - index;
    if (count <= 0) return;

    upb_Array_Delete(r->arr, index, count);
}

int PerlUpb_Repeated_Size(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    return (r && r->arr) ? upb_Array_Size(r->arr) : 0;
}

void PerlUpb_Repeated_Clear(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (r && r->arr) {
        upb_Array_Resize(r->arr, 0, NULL);
    }
}

void PerlUpb_Repeated_Free(pTHX_ SV* sv) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ sv);
    if (r) {
        SvREFCNT_dec(r->arena_sv);
        free(r);
        sv_setiv(SvRV(sv), 0);
    }
}
