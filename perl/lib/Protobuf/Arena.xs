#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/all_descriptors.h"

// -- Arena --
MODULE = Protobuf::Arena  PACKAGE = Protobuf::Arena
PROTOTYPES: ENABLE


IV
_xs_create_raw()
    CODE:
        RETVAL = PTR2IV(PerlUpb_Arena_CreateRaw(aTHX));
    OUTPUT:
        RETVAL

void
_xs_destroy_raw(ptr_iv)
    IV ptr_iv
    CODE:
        PerlUpb_Arena_DestroyRaw(aTHX_ INT2PTR(void*, ptr_iv));

