#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/port.h"

#define NUM_CACHE_STRIPES 16
#define AUDIT_LOG_SIZE 1024

typedef struct {
    int type;
    const void* ptr;
    time_t timestamp;
} obj_cache_audit_entry_t;

typedef struct {
    obj_cache_audit_entry_t entries[AUDIT_LOG_SIZE];
    size_t head;
    size_t count;
} obj_cache_audit_log_t;

static PERL_PROTOBUF_MUTEX_T cache_mutexes[NUM_CACHE_STRIPES];
static PERL_PROTOBUF_MUTEX_T lru_mutex;
static PERL_PROTOBUF_MUTEX_T audit_mutex;
static int cache_mutexes_init = 0;
static size_t max_cache_capacity = 100000;

static void ensure_mutexes_init(void) {
    if (cache_mutexes_init) return;
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_INIT(&cache_mutexes[i]);
    }
    PERL_PROTOBUF_MUTEX_INIT(&lru_mutex);
    PERL_PROTOBUF_MUTEX_INIT(&audit_mutex);
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

static AV* get_lru_av(pTHX) {
    SV* lru_sv = get_sv("Protobuf::_obj_lru", GV_ADD);
    if (!SvROK(lru_sv)) {
        AV* av = newAV();
        sv_setsv(lru_sv, newRV_noinc((SV*)av));
        return av;
    }
    return (AV*)SvRV(lru_sv);
}

static obj_cache_audit_log_t* get_audit_log(pTHX) {
    SV* audit_sv = get_sv("Protobuf::_obj_audit", GV_ADD);
    if (!SvROK(audit_sv)) {
        obj_cache_audit_log_t* log = (obj_cache_audit_log_t*)malloc(sizeof(obj_cache_audit_log_t));
        log->head = 0;
        log->count = 0;
        sv_setiv(newSVrv(audit_sv, "Protobuf::Internal::AuditLog"), (IV)log);
        return log;
    }
    return (obj_cache_audit_log_t*)SvIV(SvRV(audit_sv));
}

static void log_event(pTHX_ int type, const void* ptr) {
    ensure_mutexes_init();
    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);
    obj_cache_audit_log_t* log = get_audit_log(aTHX);
    size_t idx = (log->head + log->count) % AUDIT_LOG_SIZE;
    if (log->count == AUDIT_LOG_SIZE) {
        log->head = (log->head + 1) % AUDIT_LOG_SIZE;
    } else {
        log->count++;
    }
    log->entries[idx].type = type;
    log->entries[idx].ptr = ptr;
    log->entries[idx].timestamp = time(NULL);
    PERL_PROTOBUF_MUTEX_UNLOCK(&audit_mutex);
}

void PerlUpb_ObjCache_Init(pTHX) {
    ensure_mutexes_init();
    get_cache_hv(aTHX);
    get_lru_av(aTHX);
    get_audit_log(aTHX);
}

static void get_cache_key(const void* ptr, char* buf) {
    sprintf(buf, "%p", ptr);
}

void PerlUpb_ObjCache_SetCapacity(pTHX_ size_t capacity) {
    ensure_mutexes_init();
    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    max_cache_capacity = capacity;
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
}

size_t PerlUpb_ObjCache_GetCapacity(pTHX) {
    return max_cache_capacity;
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
    log_event(aTHX, OBJ_CACHE_EVENT_ADD, ptr);

    // LRU handling
    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    AV* lru = get_lru_av(aTHX);
    av_push(lru, newSVpv(key, 0));

    // Eviction if over capacity
    while ((size_t)av_len(lru) + 1 > max_cache_capacity) {
        SV* oldest_key_sv = av_shift(lru);
        if (oldest_key_sv && SvOK(oldest_key_sv)) {
            STRLEN len;
            const char* oldest_key = SvPV(oldest_key_sv, len);
            void* evict_ptr;
            if (sscanf(oldest_key, "%p", &evict_ptr) == 1) {
                int evict_stripe = get_stripe(evict_ptr);
                PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[evict_stripe]);
                if (hv_delete(cache, oldest_key, len, G_DISCARD)) {
                    log_event(aTHX, OBJ_CACHE_EVENT_EVICT, evict_ptr);
                }
                PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[evict_stripe]);
            }
        }
        if (oldest_key_sv) SvREFCNT_dec(oldest_key_sv);
    }
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
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
                log_event(aTHX, OBJ_CACHE_EVENT_HIT, ptr);
            }
        }
        
        if (!result) {
            hv_delete(cache, key, strlen(key), G_DISCARD);
            log_event(aTHX, OBJ_CACHE_EVENT_MISS, ptr);
        }
    } else {
        log_event(aTHX, OBJ_CACHE_EVENT_MISS, ptr);
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
    if (hv_delete(cache, key, strlen(key), G_DISCARD)) {
        log_event(aTHX, OBJ_CACHE_EVENT_DELETE, ptr);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
}

void PerlUpb_ObjCache_Clear(pTHX) {
    ensure_mutexes_init();
    // For Clear, we lock ALL stripes to ensure a consistent state
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[i]);
    }
    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);

    SV* cache_sv = get_sv("Protobuf::_obj_cache", 0);
    if (cache_sv && SvROK(cache_sv)) {
        HV* cache = (HV*)SvRV(cache_sv);
        hv_clear(cache);
    }

    SV* lru_sv = get_sv("Protobuf::_obj_lru", 0);
    if (lru_sv && SvROK(lru_sv)) {
        AV* lru = (AV*)SvRV(lru_sv);
        av_clear(lru);
    }

    obj_cache_audit_log_t* log = get_audit_log(aTHX);
    log->head = 0;
    log->count = 0;

    PERL_PROTOBUF_MUTEX_UNLOCK(&audit_mutex);
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
    for (int i = NUM_CACHE_STRIPES - 1; i >= 0; i--) {
        PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[i]);
    }
}

SV* PerlUpb_ObjCache_GetAuditLog(pTHX) {
    ensure_mutexes_init();
    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);
    obj_cache_audit_log_t* log = get_audit_log(aTHX);
    AV* av = newAV();
    for (size_t i = 0; i < log->count; i++) {
        size_t idx = (log->head + i) % AUDIT_LOG_SIZE;
        HV* entry_hv = newHV();
        hv_store(entry_hv, "type", 4, newSViv(log->entries[idx].type), 0);
        char ptr_buf[64];
        sprintf(ptr_buf, "%p", log->entries[idx].ptr);
        hv_store(entry_hv, "ptr", 3, newSVpv(ptr_buf, 0), 0);
        hv_store(entry_hv, "timestamp", 9, newSViv(log->entries[idx].timestamp), 0);
        av_push(av, newRV_noinc((SV*)entry_hv));
    }
    PERL_PROTOBUF_MUTEX_UNLOCK(&audit_mutex);
    return newRV_noinc((SV*)av);
}
