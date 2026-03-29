#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor_pool/pool.h"
#include "xs/protobuf/obj_cache.h"

SV* PerlUpb_DescriptorPool_New(pTHX) {
    upb_DefPool* pool = upb_DefPool_New();
    if (!pool) return &PL_sv_undef;
    return PerlUpb_DescriptorPool_GetWrapper(aTHX_ pool);
}

SV* PerlUpb_DescriptorPool_GetWrapper(pTHX_ const upb_DefPool* pool) {
    if (!pool) return &PL_sv_undef;

    SV* cached = PerlUpb_ObjCache_Get(aTHX_ pool);
    if (cached) {
        SvREFCNT_inc(cached);
        return cached;
    }

    SV* sv = newSViv((IV)pool);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::DescriptorPool", GV_ADD));

    PerlUpb_ObjCache_Add(aTHX_ pool, obj);
    return obj;
}

const upb_DefPool* PerlUpb_DescriptorPool_GetPool(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::DescriptorPool")) {
        return NULL;
    }
    return (const upb_DefPool*)SvIV(SvRV(sv));
}

static upb_DefPool *generated_pool = NULL;

SV* PerlUpb_DescriptorPool_GeneratedPool(pTHX) {
    if (!generated_pool) {
        // In a real implementation, this would be initialized by the upb library
        // or we'd get it from somewhere else. For now, we'll create one.
        generated_pool = upb_DefPool_New();
    }
    return PerlUpb_DescriptorPool_GetWrapper(aTHX_ generated_pool);
}

void PerlUpb_DescriptorPool_Free(pTHX_ SV* sv) {
    const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ sv);
    if (pool) {
        // Only free if it's NOT the generated pool.
        if (pool != generated_pool) {
            upb_DefPool_Free((upb_DefPool*)pool);
        }
        PerlUpb_ObjCache_Delete(aTHX_ pool);
        sv_setiv(SvRV(sv), 0);
    }
}
