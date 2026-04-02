#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/utils.h"
#include "xs/protobuf/obj_cache.h"
#include <immintrin.h>

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

SV* PerlUpb_WrapArenaBoundObject(pTHX_ const void* ptr, SV* arena_sv, const char* class_name) {
    if (!ptr) return &PL_sv_undef;

    SV* cached = PerlUpb_ObjCache_Get(aTHX_ ptr);
    if (cached) return cached;

    HV* hv = newHV();
    hv_store(hv, "_upb_ptr", 8, newSViv(PTR2IV(ptr)), 0);
    if (arena_sv && SvOK(arena_sv)) {
        hv_store(hv, "_arena_sv", 9, newSVsv(arena_sv), 0);
    }

    SV* self = newRV_noinc((SV*)hv);
    sv_bless(self, gv_stashpv(class_name, GV_ADD));
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