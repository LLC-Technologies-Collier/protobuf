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

// -- Descriptor::Enum --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::Enum
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

