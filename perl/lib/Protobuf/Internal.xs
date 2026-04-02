#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf.h"

MODULE = Protobuf::Internal  PACKAGE = Protobuf::Internal

PROTOTYPES: ENABLE

SV*
get_cache_audit_log()
    CODE:
        RETVAL = PerlUpb_ObjCache_GetAuditLog(aTHX);
    OUTPUT:
        RETVAL

void
set_cache_capacity(capacity)
    size_t capacity
    CODE:
        PerlUpb_ObjCache_SetCapacity(aTHX_ capacity);

size_t
get_cache_capacity()
    CODE:
        RETVAL = PerlUpb_ObjCache_GetCapacity(aTHX);
    OUTPUT:
        RETVAL

void
clear_cache()
    CODE:
        PerlUpb_ObjCache_Clear(aTHX);
