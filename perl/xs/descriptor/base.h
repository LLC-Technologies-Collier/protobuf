#ifndef PERL_PROTOBUF_DESCRIPTOR_BASE_H_
#define PERL_PROTOBUF_DESCRIPTOR_BASE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Common functions for all descriptor types
#define RETURN_CACHED_OR_CREATE_BLESSED(ptr, class_name) \
    do { \
        if (!(ptr)) return &PL_sv_undef; \
        SV* cached = PerlUpb_ObjCache_Get(aTHX_ (void*)(ptr)); \
        if (cached) return cached; \
        SV* sv = newSViv((IV)(ptr)); \
        SV* obj = newRV_noinc(sv); \
        sv_bless(obj, gv_stashpv(class_name, GV_ADD)); \
        PerlUpb_ObjCache_Add(aTHX_ (void*)(ptr), obj); \
        return obj; \
    } while(0)

#define EXTRACT_CACHED_DESCRIPTOR(ptr_type, sv, class_name) \
    do { \
        if (!(sv) || !SvROK(sv) || !sv_derived_from(sv, class_name)) { \
            return NULL; \
        } \
        return (const ptr_type *)SvIV(SvRV(sv)); \
    } while(0)

#endif // PERL_PROTOBUF_DESCRIPTOR_BASE_H_