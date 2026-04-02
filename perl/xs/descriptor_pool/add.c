#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor_pool/add.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor/file.h"
#include "upb/reflection/def.h"

// Note: Using the cmake-generated header as it's the most likely one to be consistent 
// with the objects we've already compiled.
#include "upb/reflection/cmake/google/protobuf/descriptor.upb.h"

SV* PerlUpb_DescriptorPool_AddSerializedFile(pTHX_ SV* self, SV* serialized) {
    if (PerlUpb_DescriptorPool_IsFrozen(aTHX_ self)) {
        croak("Cannot add file to a frozen DescriptorPool");
    }
    const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ self);
    if (!pool) return &PL_sv_undef;

    STRLEN len;
    const char* data = SvPV(serialized, len);

    upb_Arena* arena = PerlUpb_Arena_Acquire(aTHX_ PERL_UPB_LIFECYCLE_TRANSIENT);
    google_protobuf_FileDescriptorProto* proto = google_protobuf_FileDescriptorProto_parse(data, len, arena);
    if (!proto) {
        PerlUpb_Arena_Release(aTHX_ arena, PERL_UPB_LIFECYCLE_TRANSIENT);
        croak("Failed to parse FileDescriptorProto");
    }

    upb_Status status;
    upb_Status_Clear(&status);
    const upb_FileDef* file = upb_DefPool_AddFile((upb_DefPool*)pool, proto, &status);
    
    // The pool keeps its own internal state, so the proto is no longer needed.
    PerlUpb_Arena_Release(aTHX_ arena, PERL_UPB_LIFECYCLE_TRANSIENT);

    if (!file) {
        croak("Failed to add file to pool: %s", upb_Status_ErrorMessage(&status));
    }
    
    return PerlUpb_FileDef_GetWrapper(aTHX_ file);
}

SV* PerlUpb_DescriptorPool_AddSerializedFileDescriptorSet(pTHX_ SV* self, SV* serialized) {
    if (PerlUpb_DescriptorPool_IsFrozen(aTHX_ self)) {
        croak("Cannot add file to a frozen DescriptorPool");
    }
    const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ self);
    if (!pool) return &PL_sv_undef;

    STRLEN len;
    const char* data = SvPV(serialized, len);

    upb_Arena* arena = PerlUpb_Arena_Acquire(aTHX_ PERL_UPB_LIFECYCLE_TRANSIENT);
    google_protobuf_FileDescriptorSet* set = google_protobuf_FileDescriptorSet_parse(data, len, arena);
    if (!set) {
        PerlUpb_Arena_Release(aTHX_ arena, PERL_UPB_LIFECYCLE_TRANSIENT);
        croak("Failed to parse FileDescriptorSet");
    }

    size_t n;
    const google_protobuf_FileDescriptorProto* const* files = google_protobuf_FileDescriptorSet_file(set, &n);
    
    upb_Status status;
    AV* av = newAV();

    for (size_t i = 0; i < n; i++) {
        upb_Status_Clear(&status);
        const upb_FileDef* file = upb_DefPool_AddFile((upb_DefPool*)pool, files[i], &status);
        if (!file) {
            PerlUpb_Arena_Release(aTHX_ arena, PERL_UPB_LIFECYCLE_TRANSIENT);
            SvREFCNT_dec(av);
            croak("Failed to add file %zu to pool: %s", i, upb_Status_ErrorMessage(&status));
        }
        av_push(av, PerlUpb_FileDef_GetWrapper(aTHX_ file));
    }

    PerlUpb_Arena_Release(aTHX_ arena, PERL_UPB_LIFECYCLE_TRANSIENT);
    return newRV_noinc((SV*)av);
}
