#ifndef PERL_PROTOBUF_REGISTRY_H_
#define PERL_PROTOBUF_REGISTRY_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/mem/arena.h"

// Forward declaration of internal audit log type
typedef struct obj_cache_audit_log_s obj_cache_audit_log_t;

typedef struct {
    HV* obj_cache;
    AV* obj_lru;
    obj_cache_audit_log_t* audit_log;
    size_t max_cache_capacity;
    upb_Arena* cached_transient_arena;
} PerlUpb_Registry;

// Registry Management
void PerlUpb_Registry_Init(pTHX);
PerlUpb_Registry* PerlUpb_Registry_Get(pTHX);

#endif // PERL_PROTOBUF_REGISTRY_H_
