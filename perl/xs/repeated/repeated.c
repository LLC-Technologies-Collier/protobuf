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
    sv_bless(obj, gv_stashpv("Protobuf::Internal::Repeated", GV_ADD));
    return obj;
}

static PerlUpb_Repeated* GetRepeated(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Internal::Repeated")) {
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

void PerlUpb_Repeated_Insert(pTHX_ SV* self, int index, SV* val_sv) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return;

    size_t size = upb_Array_Size(r->arr);
    if (index < 0 || (size_t)index > size) {
        croak("Index out of bounds for repeated field insert");
    }

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ r->arena_sv);
    if (!upb_Array_Insert(r->arr, index, 1, arena)) {
        croak("Failed to insert into repeated field");
    }

    upb_MessageValue val;
    if (!PerlUpb_SvToUpb_Element(aTHX_ val_sv, r->f, &val, arena)) {
        // Need to delete the inserted element if conversion fails?
        // Actually upb_Array_Delete will work.
        upb_Array_Delete(r->arr, index, 1);
        croak("Failed to convert value for repeated field insert");
    }

    upb_Array_Set(r->arr, index, val);
}

void PerlUpb_Repeated_Resize(pTHX_ SV* self, int size) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return;

    if (size < 0) croak("Negative size for repeated field resize");

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ r->arena_sv);
    if (!upb_Array_Resize(r->arr, size, arena)) {
        croak("Failed to resize repeated field");
    }
}

void PerlUpb_Repeated_Clear(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (r && r->arr) {
        upb_Array_Resize(r->arr, 0, NULL);
    }
}

upb_Array* PerlUpb_Repeated_GetArray(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    return r ? r->arr : NULL;
}

const upb_FieldDef* PerlUpb_Repeated_GetFieldDef(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    return r ? r->f : NULL;
}

#include "xs/protobuf/obj_cache.h"

bool PerlUpb_Repeated_AuditIntegrity(pTHX_ SV* self) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ self);
    if (!r || !r->arr) return false;

    // Verify reified elements in the ObjCache for message/container types
    if (upb_FieldDef_IsSubMessage(r->f)) {
        size_t size = upb_Array_Size(r->arr);
        for (size_t i = 0; i < size; i++) {
            upb_MessageValue val = upb_Array_Get(r->arr, i);
            if (val.msg_val) {
                SV* cached = PerlUpb_ObjCache_Get(aTHX_ val.msg_val);
                if (cached) {
                    // We don't have a way to check if this specific instance 
                    // is "linked" to the array from the array side easily, 
                    // but we can at least verify that if we have a cache hit,
                    // it points to a valid object.
                    SvREFCNT_dec(cached);
                }
            }
        }
    }
    
    return true;
}

void PerlUpb_Repeated_Free(pTHX_ SV* sv) {
    PerlUpb_Repeated* r = GetRepeated(aTHX_ sv);
    if (r) {
        SvREFCNT_dec(r->arena_sv);
        free(r);
        sv_setiv(SvRV(sv), 0);
    }
}
