#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/registry.h"
#include "xs/protobuf/port.h"

#define NUM_CACHE_STRIPES 16
#define AUDIT_LOG_SIZE 1024

// This matches the forward decl in registry.h
struct obj_cache_audit_log_s {
    struct {
        int type;
        const void* ptr;
        time_t timestamp;
    } entries[AUDIT_LOG_SIZE];
    size_t head;
    size_t count;
};

static PERL_PROTOBUF_MUTEX_T cache_mutexes[NUM_CACHE_STRIPES];
static PERL_PROTOBUF_MUTEX_T lru_mutex;
static PERL_PROTOBUF_MUTEX_T audit_mutex;
static int cache_mutexes_init = 0;

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
    return (int)((val >> 4) % NUM_CACHE_STRIPES);
}

static HV* get_cache_hv(pTHX, PerlUpb_Registry* reg) {
    if (reg->obj_cache) return reg->obj_cache;

    SV* cache_sv = get_sv("Protobuf::_obj_cache", GV_ADD);
    if (!SvROK(cache_sv)) {
        reg->obj_cache = newHV();
        sv_setsv(cache_sv, newRV_noinc((SV*)reg->obj_cache));
    } else {
        reg->obj_cache = (HV*)SvRV(cache_sv);
    }
    return reg->obj_cache;
}

static AV* get_lru_av(pTHX, PerlUpb_Registry* reg) {
    if (reg->obj_lru) return reg->obj_lru;

    SV* lru_sv = get_sv("Protobuf::_obj_lru", GV_ADD);
    if (!SvROK(lru_sv)) {
        reg->obj_lru = newAV();
        sv_setsv(lru_sv, newRV_noinc((SV*)reg->obj_lru));
    } else {
        reg->obj_lru = (AV*)SvRV(lru_sv);
    }
    return reg->obj_lru;
}

static obj_cache_audit_log_t* get_audit_log(pTHX_ PerlUpb_Registry* reg) {
    if (reg->audit_log) return reg->audit_log;

    // Use safemalloc for the audit log to ensure it's owned by the interpreter
    reg->audit_log = (obj_cache_audit_log_t*)safemalloc(sizeof(obj_cache_audit_log_t));
    memset(reg->audit_log, 0, sizeof(obj_cache_audit_log_t));
    reg->audit_log->head = 0;
    reg->audit_log->count = 0;

    return reg->audit_log;
}


void PerlUpb_ObjCache_LogEvent(pTHX_ int type, const void* ptr) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    if (!reg) return; // Safety check
    obj_cache_audit_log_t* log = get_audit_log(aTHX, reg);
    if (!log) return; // Should not happen but safety first

    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);
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
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    get_cache_hv(aTHX, reg);
    get_lru_av(aTHX, reg);
    get_audit_log(aTHX, reg);
}

static void get_cache_key(const void* ptr, char* buf) {
    sprintf(buf, "%p", ptr);
}

void PerlUpb_ObjCache_SetCapacity(pTHX_ size_t capacity) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    reg->max_cache_capacity = capacity;
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
}

size_t PerlUpb_ObjCache_GetCapacity(pTHX) {
    return PerlUpb_Registry_Get(aTHX)->max_cache_capacity;
}

void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj) {
    if (!ptr || !obj) return;
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX, reg);
    char key[64];
    get_cache_key(ptr, key);

    SV* rv = newSVsv(obj);
    sv_rvweaken(rv);

    if (!hv_store(cache, key, strlen(key), rv, 0)) {
        SvREFCNT_dec(rv);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
    PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_ADD, ptr);

    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    AV* lru = get_lru_av(aTHX, reg);
    av_push(lru, newSVpv(key, 0));

    while ((size_t)av_len(lru) + 1 > reg->max_cache_capacity) {
        SV* oldest_key_sv = av_shift(lru);
        if (oldest_key_sv && SvOK(oldest_key_sv)) {
            STRLEN len;
            const char* oldest_key = SvPV(oldest_key_sv, len);
            void* evict_ptr;
            if (sscanf(oldest_key, "%p", &evict_ptr) == 1) {
                int evict_stripe = get_stripe(evict_ptr);
                PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[evict_stripe]);
                if (hv_delete(cache, oldest_key, len, G_DISCARD)) {
                    PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_EVICT, evict_ptr);
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
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX, reg);
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
                PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_HIT, ptr);
            }
        }
        
        if (!result) {
            hv_delete(cache, key, strlen(key), G_DISCARD);
            PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_MISS, ptr);
        }
    } else {
        PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_MISS, ptr);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
    return result;
}

void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr) {
    if (!ptr) return;
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);

    int stripe = get_stripe(ptr);
    PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[stripe]);

    HV* cache = get_cache_hv(aTHX, reg);
    char key[64];
    get_cache_key(ptr, key);
    if (hv_delete(cache, key, strlen(key), G_DISCARD)) {
        PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_DELETE, ptr);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
}

void PerlUpb_ObjCache_Clear(pTHX) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);

    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_LOCK(&cache_mutexes[i]);
    }
    PERL_PROTOBUF_MUTEX_LOCK(&lru_mutex);
    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);

    if (reg->obj_cache) {
        hv_clear(reg->obj_cache);
    }

    if (reg->obj_lru) {
        av_clear(reg->obj_lru);
    }

    if (reg->audit_log) {
        reg->audit_log->head = 0;
        reg->audit_log->count = 0;
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&audit_mutex);
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
    for (int i = NUM_CACHE_STRIPES - 1; i >= 0; i--) {
        PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[i]);
    }
}

SV* PerlUpb_ObjCache_GetAuditLog(pTHX) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    obj_cache_audit_log_t* log = get_audit_log(aTHX, reg);

    PERL_PROTOBUF_MUTEX_LOCK(&audit_mutex);
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
