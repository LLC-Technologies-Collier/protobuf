#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/port.h"

#define NUM_CACHE_STRIPES 16
static PERL_PROTOBUF_MUTEX_T cache_mutexes[NUM_CACHE_STRIPES];
static int cache_mutexes_init = 0;

static void ensure_mutexes_init(void) {
    if (cache_mutexes_init) return;
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_INIT(&cache_mutexes[i]);
    }
    cache_mutexes_init = 1;
}

static inline int get_stripe(const void* ptr) {
    uintptr_t val = (uintptr_t)ptr;
    // Simple hash for striping
    return (int)((val >> 4) % NUM_CACHE_STRIPES);
}

// The cache is a global hash (HV*) per interpreter, stored in a Perl global
// to be shared across multiple shared libraries.
static PERL_PROTOBUF_MUTEX_T global_cache_init_mutex;
static int global_cache_init_mutex_done = 0;

static HV* get_cache_hv(pTHX) {
    if (!global_cache_init_mutex_done) {
        PERL_PROTOBUF_MUTEX_INIT(&global_cache_init_mutex);
        global_cache_init_mutex_done = 1;
    }
    PERL_PROTOBUF_MUTEX_LOCK(&global_cache_init_mutex);
    SV* cache_sv = get_sv("Protobuf::_obj_cache", GV_ADD);
    HV* hv;
    if (!SvROK(cache_sv)) {
        hv = newHV();
        sv_setsv(cache_sv, newRV_noinc((SV*)hv));
    } else {
        hv = (HV*)SvRV(cache_sv);
    }
    PERL_PROTOBUF_MUTEX_UNLOCK(&global_cache_init_mutex);
    return hv;
}

void PerlUpb_ObjCache_Init(pTHX) {
    ensure_mutexes_init();
    get_cache_hv(aTHX);
}

static void get_cache_key(const void* ptr, char* buf) {
    sprintf(buf, "%p", ptr);
}

void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj) {
    if (!ptr || !obj) return;
    ensure_mutexes_init();

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX);
    char key[64];
    get_cache_key(ptr, key);

    // We store a weak reference to the object in the cache.
    SV* rv = newSVsv(obj);
    sv_rvweaken(rv);

    if (!hv_store(cache, key, strlen(key), rv, 0)) {
        SvREFCNT_dec(rv);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
}

SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr) {
    if (!ptr) return NULL;
    ensure_mutexes_init();

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX);
    char key[64];
    get_cache_key(ptr, key);

    SV** svp = hv_fetch(cache, key, strlen(key), 0);
    SV* result = NULL;

    if (svp) {
        SV* rv = *svp;
        if (rv && SvROK(rv)) {
            SV* obj = SvRV(rv);
            if (obj && obj != &PL_sv_undef) {
                result = newRV_inc(obj);
            }
        }
        
        if (!result) {
            hv_delete(cache, key, strlen(key), G_DISCARD);
        }
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
    return result;
}

void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr) {
    if (!ptr) return;
    ensure_mutexes_init();

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX);
    char key[64];
    get_cache_key(ptr, key);
    hv_delete(cache, key, strlen(key), G_DISCARD);

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
}

void PerlUpb_ObjCache_Clear(pTHX) {
    ensure_mutexes_init();
    // For Clear, we lock ALL stripes to ensure a consistent state
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[i]);
    }

    SV* cache_sv = get_sv("Protobuf::_obj_cache", 0);
    if (cache_sv && SvROK(cache_sv)) {
        HV* cache = (HV*)SvRV(cache_sv);
        hv_clear(cache);
    }

    for (int i = NUM_CACHE_STRIPES - 1; i >= 0; i--) {
        PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[i]);
    }
}
