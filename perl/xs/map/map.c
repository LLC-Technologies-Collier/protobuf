#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/map/map.h"
#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/arena.h"
#include "xs/convert.h"
#include "upb/message/map.h"
#include "upb/reflection/def.h"

typedef struct {
    upb_Map* map;
    const upb_FieldDef* f;
    SV* arena_sv;
} PerlUpb_Map;

SV* PerlUpb_Map_New(pTHX_ upb_Map* map, const upb_FieldDef* f, SV* arena_sv) {
    PerlUpb_Map* m = (PerlUpb_Map*)malloc(sizeof(PerlUpb_Map));
    m->map = map;
    m->f = f;
    m->arena_sv = newSVsv(arena_sv);

    SV* sv = newSViv((IV)m);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::Map", GV_ADD));
    return obj;
}

static PerlUpb_Map* GetMap(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::Map")) {
        return NULL;
    }
    return (PerlUpb_Map*)SvIV(SvRV(sv));
}

static void GetMapEntryDefs(const upb_FieldDef* f, const upb_FieldDef** key_f, const upb_FieldDef** val_f) {
    const upb_MessageDef* entry_def = upb_FieldDef_MessageSubDef(f);
    *key_f = upb_MessageDef_FindFieldByNumber(entry_def, 1);
    *val_f = upb_MessageDef_FindFieldByNumber(entry_def, 2);
}

SV* PerlUpb_Map_GetItem(pTHX_ SV* self, SV* key_sv) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    if (!m || !m->map) return &PL_sv_undef;

    const upb_FieldDef *key_f, *val_f;
    GetMapEntryDefs(m->f, &key_f, &val_f);

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ m->arena_sv);
    upb_MessageValue key_val;
    if (!PerlUpb_SvToUpb_Element(aTHX_ key_sv, key_f, &key_val, arena)) {
        croak("Invalid map key type");
    }

    upb_MessageValue val;
    if (upb_Map_Get(m->map, key_val, &val)) {
        return PerlUpb_UpbToSv_Element(aTHX_ &val, val_f, m->arena_sv);
    }

    return &PL_sv_undef;
}

void PerlUpb_Map_SetItem(pTHX_ SV* self, SV* key_sv, SV* value_sv) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    if (!m || !m->map) return;

    const upb_FieldDef *key_f, *val_f;
    GetMapEntryDefs(m->f, &key_f, &val_f);

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ m->arena_sv);
    upb_MessageValue key_val, val;

    if (!PerlUpb_SvToUpb_Element(aTHX_ key_sv, key_f, &key_val, arena)) {
        croak("Invalid map key type");
    }
    if (!PerlUpb_SvToUpb_Element(aTHX_ value_sv, val_f, &val, arena)) {
        croak("Invalid map value type");
    }

    upb_Map_Set(m->map, key_val, val, arena);
}

void PerlUpb_Map_DeleteItem(pTHX_ SV* self, SV* key_sv) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    if (!m || !m->map) return;

    const upb_FieldDef *key_f, *val_f;
    GetMapEntryDefs(m->f, &key_f, &val_f);

    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ m->arena_sv);
    upb_MessageValue key_val;
    if (!PerlUpb_SvToUpb_Element(aTHX_ key_sv, key_f, &key_val, arena)) {
        croak("Invalid map key type");
    }

    upb_Map_Delete(m->map, key_val, NULL);
}

void PerlUpb_Map_Clear(pTHX_ SV* self) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    if (m && m->map) {
        upb_Map_Clear(m->map);
    }
}

int PerlUpb_Map_Size(pTHX_ SV* self) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    return (m && m->map) ? upb_Map_Size(m->map) : 0;
}

void PerlUpb_Map_Free(pTHX_ SV* sv) {
    PerlUpb_Map* m = GetMap(aTHX_ sv);
    if (m) {
        SvREFCNT_dec(m->arena_sv);
        free(m);
        sv_setiv(SvRV(sv), 0);
    }
}

// Internal helper for iterator
const upb_FieldDef* PerlUpb_Map_GetFieldDef(pTHX_ SV* self) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    return m ? m->f : NULL;
}

upb_Map* PerlUpb_Map_GetMapPtr(pTHX_ SV* self) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    return m ? m->map : NULL;
}

SV* PerlUpb_Map_GetArenaSV(pTHX_ SV* self) {
    PerlUpb_Map* m = GetMap(aTHX_ self);
    return m ? m->arena_sv : NULL;
}
