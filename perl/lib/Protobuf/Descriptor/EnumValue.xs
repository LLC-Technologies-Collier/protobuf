#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/all_descriptors.h"

// -- Descriptor::EnumValue --
MODULE = Protobuf::Descriptor::EnumValue  PACKAGE = Protobuf::Descriptor::EnumValue
PROTOTYPES: ENABLE

const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_EnumValueDef* ev = PerlUpb_EnumValueDef_GetEnumValue(aTHX_ self);
        RETVAL = ev ? upb_EnumValueDef_Name(ev) : NULL;
    OUTPUT:
        RETVAL

int
_xs_number(self)
    SV* self
    CODE:
        const upb_EnumValueDef* ev = PerlUpb_EnumValueDef_GetEnumValue(aTHX_ self);
        RETVAL = ev ? upb_EnumValueDef_Number(ev) : 0;
    OUTPUT:
        RETVAL

int
_xs_index(self)
    SV* self
    CODE:
        const upb_EnumValueDef* ev = PerlUpb_EnumValueDef_GetEnumValue(aTHX_ self);
        RETVAL = ev ? upb_EnumValueDef_Index(ev) : 0;
    OUTPUT:
        RETVAL
