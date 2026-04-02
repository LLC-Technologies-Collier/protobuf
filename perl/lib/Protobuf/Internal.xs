#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf.h"
#include "perl/xs/protobuf/utils.h"

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

SV*
class_name_to_full_name(class_name)
    const char* class_name
    PREINIT:
        char* full_name;
    CODE:
        full_name = PerlUpb_ClassNameToFullName(aTHX_ class_name);
        RETVAL = newSVpv(full_name, 0);
        safefree(full_name);
    OUTPUT:
        RETVAL

SV*
full_name_to_class_name(full_name_str)
    const char* full_name_str
    PREINIT:
        char* class_name;
    CODE:
        class_name = PerlUpb_FullNameToClassName(aTHX_ full_name_str);
        RETVAL = newSVpv(class_name, 0);
        safefree(class_name);
    OUTPUT:
        RETVAL
