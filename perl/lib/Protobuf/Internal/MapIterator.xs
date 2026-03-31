#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/map/iterator.h"

MODULE = Protobuf::Internal::MapIterator  PACKAGE = Protobuf::Internal::MapIterator
PROTOTYPES: ENABLE

SV*
_xs_next_key(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Map_Iterator_NextKey(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_next_value(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Map_Iterator_Value(aTHX_ self);
    OUTPUT:
        RETVAL

void
DESTROY(self)
    SV* self
    CODE:
        extern void PerlUpb_MapIterator_Free(pTHX_ SV* sv);
        PerlUpb_MapIterator_Free(aTHX_ self);
