#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

#include "xs/protobuf/obj_cache.h"

// The cache is a global hash (HV*) per interpreter
static HV* g_obj_cache = NULL;

void PerlUpb_ObjCache_Init(pTHX) {
    if (!g_obj_cache) {
        g_obj_cache = newHV();
    }
}

static void get_cache_key(const void* ptr, char* buf) {
    sprintf(buf, "%p", ptr);
}

void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj) {
    if (!ptr || !obj) return;
    PerlUpb_ObjCache_Init(aTHX);

    char key[64];
    get_cache_key(ptr, key);

    SV* key_sv = newSVpv(key, 0);
    
    // We store a weak reference to the object in the cache.
    // The 'obj' passed in is expected to be a reference (RV) to the blessed SV.
    SV* rv = newSVsv(obj);
    sv_rvweaken(rv);

    if (!hv_store_ent(g_obj_cache, key_sv, rv, 0)) {
        SvREFCNT_dec(rv);
    }
    SvREFCNT_dec(key_sv);
}

SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr) {
    if (!ptr || !g_obj_cache) return NULL;

    char key[64];
    get_cache_key(ptr, key);

    SV** svp = hv_fetch(g_obj_cache, key, strlen(key), 0);
    if (!svp) return NULL;

    SV* rv = *svp;
    if (rv && SvROK(rv)) {
        SV* obj = SvRV(rv);
        if (obj && SvOK(obj)) {
            // Found a valid cached object. Return a NEW reference to it.
            return newRV_inc(obj);
        }
    }
    
    // If we reach here, the weak ref was collected or the SV is invalid.
    // We should probably clean up the entry.
    hv_delete(g_obj_cache, key, strlen(key), G_DISCARD);
    return NULL;
}

void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr) {
    if (!ptr || !g_obj_cache) return;

    char key[64];
    get_cache_key(ptr, key);
    hv_delete(g_obj_cache, key, strlen(key), G_DISCARD);
}

void PerlUpb_ObjCache_Clear(pTHX) {
    if (g_obj_cache) {
        hv_clear(g_obj_cache);
        SvREFCNT_dec(g_obj_cache);
        g_obj_cache = NULL;
    }
}
