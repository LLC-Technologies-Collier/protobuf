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

typedef struct {
    uint64_t acquisitions;
    uint64_t contentions;
} contention_stat_t;

static struct {
    contention_stat_t stripes[NUM_CACHE_STRIPES];
    contention_stat_t lru;
    contention_stat_t audit;
} contention_stats;

static void LOCK_AND_PROFILE(PERL_PROTOBUF_MUTEX_T* m, contention_stat_t* s) {
    if (PERL_PROTOBUF_MUTEX_TRYLOCK(m)) {
        s->acquisitions++;
    } else {
        s->contentions++;
        s->acquisitions++;
        PERL_PROTOBUF_MUTEX_LOCK(m);
    }
}

static void ensure_mutexes_init(void) {
    if (cache_mutexes_init) return;
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        PERL_PROTOBUF_MUTEX_INIT(&cache_mutexes[i]);
    }
    PERL_PROTOBUF_MUTEX_INIT(&lru_mutex);
    PERL_PROTOBUF_MUTEX_INIT(&audit_mutex);
    memset(&contention_stats, 0, sizeof(contention_stats));
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

    LOCK_AND_PROFILE(&audit_mutex, &contention_stats.audit);
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

static inline void get_cache_key(const void* ptr, char* buf) {
    memcpy(buf, &ptr, sizeof(void*));
}
#define CACHE_KEY_LEN sizeof(void*)

void PerlUpb_ObjCache_SetCapacity(pTHX_ size_t capacity) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    LOCK_AND_PROFILE(&lru_mutex, &contention_stats.lru);
    reg->max_cache_capacity = capacity;
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
}

size_t PerlUpb_ObjCache_GetCapacity(pTHX) {
    return PerlUpb_Registry_Get(aTHX)->max_cache_capacity;
}

void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj) {
    if (!ptr || !obj || !SvROK(obj)) return;
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    if (!reg) return;

    int stripe = get_stripe(ptr);
    LOCK_AND_PROFILE(&cache_mutexes[stripe], &contention_stats.stripes[stripe]);

    HV* cache = get_cache_hv(aTHX, reg);
    char key[CACHE_KEY_LEN];
    get_cache_key(ptr, key);

    // Create a NEW reference to the target object (the HV) and weaken it
    SV* target = SvRV(obj);
    SV* weak_rv = newRV_inc(target);
    sv_rvweaken(weak_rv);

    if (!hv_store(cache, key, CACHE_KEY_LEN, weak_rv, 0)) {
        SvREFCNT_dec(weak_rv);
    }

    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[stripe]);
    PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_ADD, ptr);

    LOCK_AND_PROFILE(&lru_mutex, &contention_stats.lru);
    AV* lru = get_lru_av(aTHX, reg);
    av_push(lru, newSVpvn(key, CACHE_KEY_LEN));

    while ((size_t)av_len(lru) + 1 > reg->max_cache_capacity) {
        SV* oldest_key_sv = av_shift(lru);
        if (oldest_key_sv && SvOK(oldest_key_sv)) {
            STRLEN len;
            const char* oldest_key = SvPVbyte(oldest_key_sv, len);
            void* evict_ptr;
            if (len == CACHE_KEY_LEN) {
                memcpy(&evict_ptr, oldest_key, CACHE_KEY_LEN);
                int evict_stripe = get_stripe(evict_ptr);
                LOCK_AND_PROFILE(&cache_mutexes[evict_stripe], &contention_stats.stripes[evict_stripe]);
                hv_delete(cache, oldest_key, len, G_DISCARD);
                PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_EVICT, evict_ptr);
                PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[evict_stripe]);
            }
        }
        if (oldest_key_sv && !PL_dirty) SvREFCNT_dec(oldest_key_sv);
    }
    PERL_PROTOBUF_MUTEX_UNLOCK(&lru_mutex);
}

SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr) {
    if (!ptr) return NULL;
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    if (!reg) return NULL;

    int stripe = get_stripe(ptr);
    LOCK_AND_PROFILE(&cache_mutexes[stripe], &contention_stats.stripes[stripe]);

    HV* cache = get_cache_hv(aTHX, reg);
    char key[CACHE_KEY_LEN];
    get_cache_key(ptr, key);

    SV** svp = hv_fetch(cache, key, CACHE_KEY_LEN, 0);
    SV* result = NULL;

    if (svp) {
        SV* rv = *svp;
        if (rv && SvROK(rv)) {
            SV* target = SvRV(rv);
            if (target && target != &PL_sv_undef) {
                // Return a NEW strong reference (RV) to the object
                result = newRV_inc(target);
                PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_HIT, ptr);
            }
        }
        
        if (!result) {
            // Weak ref was collected or invalid, cleanup entry
            hv_delete(cache, key, CACHE_KEY_LEN, G_DISCARD);
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
    char key[CACHE_KEY_LEN];
    get_cache_key(ptr, key);
    PerlUpb_ObjCache_DeleteEntry(aTHX_ key, CACHE_KEY_LEN);
}

void PerlUpb_ObjCache_DeleteEntry(pTHX_ const char* key_bytes, STRLEN len) {
    if (!key_bytes || len != CACHE_KEY_LEN) return;
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);
    if (!reg) return;

    void* target_ptr;
    memcpy(&target_ptr, key_bytes, CACHE_KEY_LEN);

    bool deleted = false;
    
    // First try the expected stripe
    int expected_stripe = get_stripe(target_ptr);
    LOCK_AND_PROFILE(&cache_mutexes[expected_stripe], &contention_stats.stripes[expected_stripe]);
    HV* cache = get_cache_hv(aTHX, reg);
    if (hv_exists(cache, key_bytes, CACHE_KEY_LEN)) {
        hv_delete(cache, key_bytes, CACHE_KEY_LEN, G_DISCARD);
        deleted = true;
    }
    PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[expected_stripe]);

    if (!deleted) {
        // Fallback: Check ALL stripes
        for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
            if (i == expected_stripe) continue;
            LOCK_AND_PROFILE(&cache_mutexes[i], &contention_stats.stripes[i]);
            if (hv_exists(cache, key_bytes, CACHE_KEY_LEN)) {
                hv_delete(cache, key_bytes, CACHE_KEY_LEN, G_DISCARD);
                deleted = true;
                PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[i]);
                break;
            }
            PERL_PROTOBUF_MUTEX_UNLOCK(&cache_mutexes[i]);
        }
    }

    if (deleted) {
        PerlUpb_ObjCache_LogEvent(aTHX, OBJ_CACHE_EVENT_DELETE, target_ptr);
    }
}

void PerlUpb_ObjCache_Clear(pTHX) {
    ensure_mutexes_init();
    PerlUpb_Registry* reg = PerlUpb_Registry_Get(aTHX);

    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        LOCK_AND_PROFILE(&cache_mutexes[i], &contention_stats.stripes[i]);
    }
    LOCK_AND_PROFILE(&lru_mutex, &contention_stats.lru);
    LOCK_AND_PROFILE(&audit_mutex, &contention_stats.audit);

    if (!PL_dirty) {
        if (reg->obj_cache) {
            hv_clear(reg->obj_cache);
        }

        if (reg->obj_lru) {
            av_clear(reg->obj_lru);
        }
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

    LOCK_AND_PROFILE(&audit_mutex, &contention_stats.audit);
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

SV* PerlUpb_ObjCache_GetContentionStats(pTHX) {
    ensure_mutexes_init();
    HV* hv = newHV();

    AV* stripes_av = newAV();
    for (int i = 0; i < NUM_CACHE_STRIPES; i++) {
        HV* s_hv = newHV();
        hv_store(s_hv, "acquisitions", 12, newSVuv(contention_stats.stripes[i].acquisitions), 0);
        hv_store(s_hv, "contentions", 11, newSVuv(contention_stats.stripes[i].contentions), 0);
        av_push(stripes_av, newRV_noinc((SV*)s_hv));
    }
    hv_store(hv, "stripes", 7, newRV_noinc((SV*)stripes_av), 0);

    HV* lru_hv = newHV();
    hv_store(lru_hv, "acquisitions", 12, newSVuv(contention_stats.lru.acquisitions), 0);
    hv_store(lru_hv, "contentions", 11, newSVuv(contention_stats.lru.contentions), 0);
    hv_store(hv, "lru", 3, newRV_noinc((SV*)lru_hv), 0);

    HV* audit_hv = newHV();
    hv_store(audit_hv, "acquisitions", 12, newSVuv(contention_stats.audit.acquisitions), 0);
    hv_store(audit_hv, "contentions", 11, newSVuv(contention_stats.audit.contentions), 0);
    hv_store(hv, "audit", 5, newRV_noinc((SV*)audit_hv), 0);

    return newRV_noinc((SV*)hv);
}
