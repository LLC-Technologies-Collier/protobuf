#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

#include "xs/protobuf/obj_cache.h"

// The cache is a global hash (HV*) per interpreter, stored in a Perl global
// to be shared across multiple shared libraries.
static HV* get_cache_hv(pTHX) {
    SV* cache_sv = get_sv("Protobuf::_obj_cache", GV_ADD);
    if (!SvROK(cache_sv)) {
        HV* hv = newHV();
        sv_setsv(cache_sv, newRV_noinc((SV*)hv));
        return hv;
    }
    return (HV*)SvRV(cache_sv);
}

void PerlUpb_ObjCache_Init(pTHX) {
    get_cache_hv(aTHX);
}

static void get_cache_key(const void* ptr, char* buf) {
    sprintf(buf, "%p", ptr);
}

void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj) {
    if (!ptr || !obj) return;
    HV* cache = get_cache_hv(aTHX);

    char key[64];
    get_cache_key(ptr, key);

    // We store a weak reference to the object in the cache.
    SV* rv = newSVsv(obj);
    sv_rvweaken(rv);

    if (!hv_store(cache, key, strlen(key), rv, 0)) {
        SvREFCNT_dec(rv);
    }
}

SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr) {
    if (!ptr) return NULL;
    HV* cache = get_cache_hv(aTHX);

    char key[64];
    get_cache_key(ptr, key);

    SV** svp = hv_fetch(cache, key, strlen(key), 0);
    if (!svp) return NULL;

    SV* rv = *svp;
    if (rv && SvROK(rv)) {
        SV* obj = SvRV(rv);
        if (obj && obj != &PL_sv_undef) {
            return newRV_inc(obj);
        }
    }
    
    hv_delete(cache, key, strlen(key), G_DISCARD);
    return NULL;
}

void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr) {
    if (!ptr) return;
    HV* cache = get_cache_hv(aTHX);

    char key[64];
    get_cache_key(ptr, key);
    hv_delete(cache, key, strlen(key), G_DISCARD);
}

void PerlUpb_ObjCache_Clear(pTHX) {
    SV* cache_sv = get_sv("Protobuf::_obj_cache", 0);
    if (cache_sv && SvROK(cache_sv)) {
        HV* cache = (HV*)SvRV(cache_sv);
        hv_clear(cache);
    }
}
