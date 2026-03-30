#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/message/serialize.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor_pool/add.h"
#include "xs/descriptor_pool/find.h"
#include "xs/descriptor/file.h"
#include "xs/descriptor/message.h"
#include "xs/descriptor/enum.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/enum_value.h"
#include "xs/descriptor/oneof.h"
#include "xs/descriptor/service.h"
#include "xs/descriptor/method.h"

// -- Arena --
MODULE = Protobuf  PACKAGE = Protobuf::Arena
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

