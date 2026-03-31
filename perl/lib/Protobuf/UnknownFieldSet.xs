#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/unknown_fields/set.h"

MODULE = Protobuf::UnknownFieldSet  PACKAGE = Protobuf::UnknownFieldSet
PROTOTYPES: ENABLE

SV*
_xs_get_data(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_UnknownFieldSet_GetData(aTHX_ self);
    OUTPUT:
        RETVAL

void
_xs_add(self, data)
    SV* self
    SV* data
    CODE:
        PerlUpb_UnknownFieldSet_Add(aTHX_ self, data);

void
_xs_clear(self)
    SV* self
    CODE:
        PerlUpb_UnknownFieldSet_Clear(aTHX_ self);

void
DESTROY(self)
    SV* self
    CODE:
        extern void PerlUpb_UnknownFieldSet_Free(pTHX_ SV* sv);
        PerlUpb_UnknownFieldSet_Free(aTHX_ self);
