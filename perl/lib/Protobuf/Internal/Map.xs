#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/map/map.h"
#include "xs/map/iterator.h"

MODULE = Protobuf::Internal::Map  PACKAGE = Protobuf::Internal::Map
PROTOTYPES: ENABLE

SV*
_xs_get_item(self, key)
    SV* self
    SV* key
    CODE:
        RETVAL = PerlUpb_Map_GetItem(aTHX_ self, key);
    OUTPUT:
        RETVAL

void
_xs_set_item(self, key, value)
    SV* self
    SV* key
    SV* value
    CODE:
        PerlUpb_Map_SetItem(aTHX_ self, key, value);

void
_xs_delete_item(self, key)
    SV* self
    SV* key
    CODE:
        PerlUpb_Map_DeleteItem(aTHX_ self, key);

int
_xs_size(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Map_Size(aTHX_ self);
    OUTPUT:
        RETVAL

void
_xs_clear(self)
    SV* self
    CODE:
        PerlUpb_Map_Clear(aTHX_ self);

SV*
_xs_new_iterator(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Map_GetIterator(aTHX_ self);
    OUTPUT:
        RETVAL

void
DESTROY(self)
    SV* self
    CODE:
        PerlUpb_Map_Free(aTHX_ self);

