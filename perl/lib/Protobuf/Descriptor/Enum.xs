#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/all_descriptors.h"

// -- Descriptor::Enum --
MODULE = Protobuf::Descriptor::Enum  PACKAGE = Protobuf::Descriptor::Enum
PROTOTYPES: ENABLE


const char*
_xs_full_name(self)
    SV* self
    CODE:
        const upb_EnumDef* e = PerlUpb_EnumDef_GetEnum(aTHX_ self);
        RETVAL = e ? upb_EnumDef_FullName(e) : NULL;
    OUTPUT:
        RETVAL

const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_EnumDef* e = PerlUpb_EnumDef_GetEnum(aTHX_ self);
        RETVAL = e ? upb_EnumDef_Name(e) : NULL;
    OUTPUT:
        RETVAL

int
_xs_value_count(self)
    SV* self
    CODE:
        const upb_EnumDef* e = PerlUpb_EnumDef_GetEnum(aTHX_ self);
        RETVAL = e ? upb_EnumDef_ValueCount(e) : 0;
    OUTPUT:
        RETVAL

const char*
_xs_value_name(self, index)
    SV* self
    int index
    CODE:
        const upb_EnumDef* e = PerlUpb_EnumDef_GetEnum(aTHX_ self);
        if (e && index >= 0 && index < upb_EnumDef_ValueCount(e)) {
            const upb_EnumValueDef* ev = upb_EnumDef_Value(e, index);
            RETVAL = upb_EnumValueDef_Name(ev);
        } else {
            RETVAL = NULL;
        }
    OUTPUT:
        RETVAL

