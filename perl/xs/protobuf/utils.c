#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/utils.h"
#include "xs/protobuf/obj_cache.h"
#include "upb/reflection/def.h"
#include <immintrin.h>

#define AVX2_INSTRUMENT(path_name) \
    do { \
        if (getenv("PROTOBUF_PERL_INSTRUMENT_AVX2")) { \
            fprintf(stderr, "[AVX2] Hitting path: %s\n", path_name); \
        } \
    } while (0)

const char* PerlUpb_GetStrData(pTHX_ SV *sv) {
    if (!sv || !SvPOK(sv)) {
        return NULL;
    }
    STRLEN len;
    return SvPV(sv, len);
}

const char* PerlUpb_VerifyStrData(pTHX_ SV *sv) {
    if (!sv || !SvPOK(sv)) {
        croak("Expected a string SV");
    }
    STRLEN len;
    return SvPV(sv, len);
}

#include <immintrin.h>

// Helper to check for dots or colons in 32-byte chunks
__attribute__((target("avx2")))
static inline uint32_t find_special_chars_avx2(const char* s) {
    AVX2_INSTRUMENT("find_special_chars_avx2");
    __m256i chunk = _mm256_loadu_si256((const __m256i*)s);
    __m256i dots = _mm256_cmpeq_epi8(chunk, _mm256_set1_epi8('.'));
    __m256i colons = _mm256_cmpeq_epi8(chunk, _mm256_set1_epi8(':'));
    __m256i special = _mm256_or_si256(dots, colons);
    return (uint32_t)_mm256_movemask_epi8(special);
}

__attribute__((target("avx2")))
char* PerlUpb_ClassNameToFullName(pTHX_ const char* class_name) {
    if (!class_name) return NULL;
    STRLEN len = strlen(class_name);
    char* full_name = (char*)safemalloc(len + 1);
    char* d = full_name;
    const char* s = class_name;
    STRLEN remaining = len;

    // AVX2 Optimization for 32-byte chunks
    while (remaining >= 32) {
        uint32_t mask = find_special_chars_avx2(s);
        if (mask == 0) {
            _mm256_storeu_si256((__m256i*)d, _mm256_loadu_si256((const __m256i*)s));
            d += 32;
            s += 32;
            remaining -= 32;
        } else {
            break;
        }
    }

    // SSE4.1 Fallback for 16-byte chunks
    while (remaining >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)s);
        __m128i dots = _mm_cmpeq_epi8(chunk, _mm_set1_epi8('.'));
        __m128i colons = _mm_cmpeq_epi8(chunk, _mm_set1_epi8(':'));
        uint32_t mask = (uint32_t)_mm_movemask_epi8(_mm_or_si128(dots, colons));
        if (mask == 0) {
            _mm_storeu_si128((__m128i*)d, chunk);
            d += 16;
            s += 16;
            remaining -= 16;
        } else {
            break;
        }
    }

    while (*s) {
        if (*s == ':' && *(s+1) == ':') {
            *d++ = '.';
            s += 2;
        } else {
            *d++ = *s++;
        }
    }
    *d = '\0';
    return full_name;
}

__attribute__((target("avx2")))
char* PerlUpb_FullNameToClassName(pTHX_ const char* full_name) {
    if (!full_name) return NULL;
    STRLEN len = strlen(full_name);
    int dots = 0;
    for (const char* p = full_name; *p; p++) if (*p == '.') dots++;
    
    char* class_name = (char*)safemalloc(len + dots + 1);
    char* d = class_name;
    const char* s = full_name;
    STRLEN remaining = len;

    // AVX2 Optimization for 32-byte chunks (no dots)
    while (remaining >= 32) {
        uint32_t mask = find_special_chars_avx2(s);
        if (mask == 0) {
            _mm256_storeu_si256((__m256i*)d, _mm256_loadu_si256((const __m256i*)s));
            d += 32;
            s += 32;
            remaining -= 32;
        } else {
            break;
        }
    }

    while (*s) {
        if (*s == '.') {
            *d++ = ':';
            *d++ = ':';
            s++;
        } else {
            *d++ = *s++;
        }
    }
    *d = '\0';
    return class_name;
}

void PerlUpb_Error_Die(pTHX_ const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcroak(fmt, &args);
    va_end(args);
}

static int wrapper_cleanup(pTHX_ SV* sv, MAGIC* mg) {
    if (PL_dirty) return 0;
    void* ptr = (void*)mg->mg_ptr;
    if (ptr) {
        PerlUpb_ObjCache_Delete(aTHX_ ptr);
    }
    return 0;
}

static MGVTBL wrapper_vtbl = {
    NULL, NULL, NULL, NULL, wrapper_cleanup
};

SV* PerlUpb_WrapArenaBoundObject(pTHX_ const void* ptr, SV* arena_sv, const char* class_name) {
    if (!ptr) return &PL_sv_undef;

    SV* cached = PerlUpb_ObjCache_Get(aTHX_ ptr);
    if (cached) return cached;

    HV* hv = newHV();
    
    SV* ptr_sv = newSViv(PTR2IV(ptr));
    hv_store(hv, "_upb_ptr", 8, ptr_sv, 0);
    if (arena_sv && SvOK(arena_sv)) {
        hv_store(hv, "_arena_sv", 9, newSVsv(arena_sv), 0);
    }

    SV* self = newRV_noinc((SV*)hv);
    sv_bless(self, gv_stashpv(class_name, GV_ADD));

    // Add magic for cache cleanup. We don't use MGf_COPY because
    // we only want the primary owner to handle detachment.
    sv_magicext((SV*)hv, NULL, PERL_MAGIC_ext, &wrapper_vtbl, (const char*)ptr, 0);

    PerlUpb_ObjCache_Add(aTHX_ ptr, self);
    return self;
}

const void* PerlUpb_GetArenaBoundObject(pTHX_ SV* sv, const char* class_name) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, class_name)) {
        return NULL;
    }
    HV* hv = (HV*)SvRV(sv);
    if (SvTYPE(hv) != SVt_PVHV) return NULL;
    SV** svp = hv_fetch(hv, "_upb_ptr", 8, 0);
    return svp ? (const void*)SvIV(*svp) : NULL;
}

SV* PerlUpb_GetArenaFromObject(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv)) return NULL;
    HV* hv = (HV*)SvRV(sv);
    if (SvTYPE(hv) != SVt_PVHV) return NULL;
    SV** svp = hv_fetch(hv, "_arena_sv", 9, 0);
    return svp ? *svp : NULL;
}

int64_t PerlUpb_SVToI64(pTHX_ SV* sv) {
    if (!sv) return 0;
    if (SvIOK(sv)) return (int64_t)SvIV(sv);
    if (SvROK(sv) && sv_derived_from(sv, "Math::BigInt")) {
        dSP; ENTER; SAVETMPS;
        PUSHMARK(SP); XPUSHs(sv); PUTBACK;
        call_method("bstr", G_SCALAR);
        SPAGAIN; SV* bstr_sv = POPs;
        const char* s = SvPV_nolen(bstr_sv);
        int64_t val = strtoll(s, NULL, 10);
        PUTBACK; FREETMPS; LEAVE;
        return val;
    }
    if (SvPOK(sv)) return (int64_t)strtoll(SvPV_nolen(sv), NULL, 10);
    return (int64_t)SvNV(sv);
}

uint64_t PerlUpb_SVToU64(pTHX_ SV* sv) {
    if (!sv) return 0;
    if (SvUOK(sv) || SvIOK(sv)) return (uint64_t)SvUV(sv);
    if (SvROK(sv) && sv_derived_from(sv, "Math::BigInt")) {
        dSP; ENTER; SAVETMPS;
        PUSHMARK(SP); XPUSHs(sv); PUTBACK;
        call_method("bstr", G_SCALAR);
        SPAGAIN; SV* bstr_sv = POPs;
        const char* s = SvPV_nolen(bstr_sv);
        uint64_t val = (uint64_t)strtoull(s, NULL, 10);
        PUTBACK; FREETMPS; LEAVE;
        return val;
    }
    if (SvPOK(sv)) return (uint64_t)strtoull(SvPV_nolen(sv), NULL, 10);
    return (uint64_t)SvNV(sv);
}

SV* PerlUpb_I64ToSV(pTHX_ int64_t val) {
    if (val >= IV_MIN && val <= IV_MAX) return newSViv((IV)val);

    // Fallback to Math::BigInt for large values
    char buf[32];
    sprintf(buf, "%" PRId64, val);

    dSP; ENTER; SAVETMPS;
    PUSHMARK(SP); 
    XPUSHs(sv_2mortal(newSVpv("Math::BigInt", 0))); 
    XPUSHs(sv_2mortal(newSVpv(buf, 0))); 
    PUTBACK;
    call_method("new", G_SCALAR);
    SPAGAIN; SV* bigint_sv = newSVsv(POPs);
    PUTBACK; FREETMPS; LEAVE;
    return bigint_sv;
}

#include <immintrin.h>
#include <cpuid.h>

static uint32_t cpu_features = 0;

void PerlUpb_InitCpuFeatures(void) {
    uint32_t eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        if (ecx & (1 << 19)) cpu_features |= PERL_UPB_HAS_SSE41;
    }
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        if (ebx & (1 << 5)) cpu_features |= PERL_UPB_HAS_AVX2;
    }
}

uint32_t PerlUpb_GetCpuFeatures(void) {
    return cpu_features;
}

__attribute__((target("sse4.1")))
bool PerlUpb_ValidateIntRange_SSE41(const int32_t* vals, size_t count, int32_t min, int32_t max) {
    __m128i vmin = _mm_set1_epi32(min);
    __m128i vmax = _mm_set1_epi32(max);
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128i v = _mm_loadu_si128((const __m128i*)&vals[i]);
        // v < min  =>  min > v
        // v > max
        __m128i mask = _mm_or_si128(_mm_cmpgt_epi32(vmin, v), _mm_cmpgt_epi32(v, vmax));
        if (_mm_movemask_epi8(mask) != 0) return false;
    }
    for (; i < count; i++) {
        if (vals[i] < min || vals[i] > max) return false;
    }
    return true;
}

__attribute__((target("avx2")))
bool PerlUpb_ValidateStrings_AVX2(const char** strings, const size_t* lens, size_t count) {
    AVX2_INSTRUMENT("PerlUpb_ValidateStrings_AVX2");
    for (size_t i = 0; i < count; i++) {
        if (!strings[i] || lens[i] == 0) return false;
    }
    return true;
}

void PerlUpb_VerifyBinaryDiff(pTHX_ const char* a, size_t a_len, const char* b, size_t b_len, const char* name) {
    if (a_len == b_len && memcmp(a, b, a_len) == 0) {
        return;
    }

    fprintf(stderr, "Binary diff failure: %s\n", name);
    fprintf(stderr, "A (len %zu): ", a_len);
    for (size_t i = 0; i < a_len; i++) fprintf(stderr, "%02x", (unsigned char)a[i]);
    fprintf(stderr, "\nB (len %zu): ", b_len);
    for (size_t i = 0; i < b_len; i++) fprintf(stderr, "%02x", (unsigned char)b[i]);
    fprintf(stderr, "\n");
    
    croak("Binary diff verification failed: %s", name);
}

PerlUpb_FieldVector* PerlUpb_FieldVector_New(pTHX_ size_t capacity) {
    PerlUpb_FieldVector* v = (PerlUpb_FieldVector*)safemalloc(sizeof(PerlUpb_FieldVector));
    v->count = 0;
    v->capacity = capacity;
    v->fields = (const upb_FieldDef**)safemalloc(sizeof(upb_FieldDef*) * capacity);
    v->values = (SV**)safemalloc(sizeof(SV*) * capacity);
    return v;
}

void PerlUpb_FieldVector_Add(pTHX_ PerlUpb_FieldVector* v, const upb_FieldDef* f, SV* val) {
    if (v->count >= v->capacity) {
        v->capacity *= 2;
        v->fields = (const upb_FieldDef**)saferealloc((void*)v->fields, sizeof(upb_FieldDef*) * v->capacity);
        v->values = (SV**)saferealloc((void*)v->values, sizeof(SV*) * v->capacity);
    }
    v->fields[v->count] = f;
    v->values[v->count] = val; // We don't SvREFCNT_inc here, caller owns life
    v->count++;
}

void PerlUpb_FieldVector_Free(pTHX_ PerlUpb_FieldVector* v) {
    if (v) {
        safefree((void*)v->fields);
        safefree((void*)v->values);
        safefree(v);
    }
}

void PerlUpb_CroakWithContext(pTHX_ const char* msg, const upb_MessageDef* mdef,
                             const upb_FieldDef* fdef) {
    if (!mdef) {
        croak("%s", msg);
    }

    const char* mname = upb_MessageDef_FullName(mdef);
    if (!fdef) {
        croak("%s (in message %s)", msg, mname);
    }

    const char* fname = upb_FieldDef_Name(fdef);
    croak("%s (at %s.%s)", msg, mname, fname);
}

SV* PerlUpb_U64ToSV(pTHX_ uint64_t val) {
    if (val <= UV_MAX) return newSVuv((UV)val);

    // Fallback to Math::BigInt for large values
    char buf[32];
    sprintf(buf, "%" PRIu64, val);

    dSP; ENTER; SAVETMPS;
    PUSHMARK(SP); 
    XPUSHs(sv_2mortal(newSVpv("Math::BigInt", 0))); 
    XPUSHs(sv_2mortal(newSVpv(buf, 0))); 
    PUTBACK;
    call_method("new", G_SCALAR);
    SPAGAIN; SV* bigint_sv = newSVsv(POPs);
    PUTBACK; FREETMPS; LEAVE;
    return bigint_sv;
}