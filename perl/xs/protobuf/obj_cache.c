#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/obj_cache.h"
#include <string.h>

// Global hash for object cache
static HV *obj_cache = NULL;

// Initialize the cache
void protobuf_init_obj_cache(pTHX) {
    if (!obj_cache) {
        obj_cache = newHV();
    }
}

// Add an object to the cache
void protobuf_register_object(pTHX_ const char *key, SV *value) {
    if (!obj_cache) protobuf_init_obj_cache(aTHX);

    SV *key_sv = newSVpvn(key, strlen(key));
    SV *val_ref = newRV_inc(value);
    
    // Weaken the reference so the cache doesn't prevent garbage collection
    sv_rvweaken(val_ref);

    if (!hv_store_ent(obj_cache, key_sv, val_ref, 0)) {
      SvREFCNT_dec(val_ref);
      SvREFCNT_dec(key_sv);
    }
}

// Get an object from the cache
SV *protobuf_get_object(pTHX_ const char *key) {
    if (!obj_cache) return NULL;

    SV *key_sv = newSVpvn(key, strlen(key));
    HE *he = hv_fetch_ent(obj_cache, key_sv, 0, 0);
    SvREFCNT_dec(key_sv);

    if (he) {
        SV *val_ref = HeVAL(he);
        if (val_ref && SvROK(val_ref)) {
            SV *cached_sv = SvRV(val_ref);
            if (cached_sv && SvOK(cached_sv)) { // Ensure it wasn't destroyed
                return newSVsv(cached_sv);
            }
        }
    }
    return NULL;
}

// Remove an object from the cache
void protobuf_unregister_object(pTHX_ const char *key) {
    if (!obj_cache) return;
    hv_delete(obj_cache, key, strlen(key), G_DISCARD);
}

void protobuf_clear_obj_cache(PerlInterpreter *my_perl) {
    if (obj_cache) {
        hv_clear(obj_cache);
        SvREFCNT_dec(obj_cache);
        obj_cache = NULL;
    }
}
