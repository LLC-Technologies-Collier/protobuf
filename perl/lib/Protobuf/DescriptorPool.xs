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

// -- DescriptorPool --
MODULE = Protobuf  PACKAGE = Protobuf::DescriptorPool
PROTOTYPES: ENABLE


IV
_xs_create_raw()
    CODE:
        RETVAL = PTR2IV(PerlUpb_DescriptorPool_CreateRaw(aTHX));
    OUTPUT:
        RETVAL

void
_xs_destroy_raw(ptr_iv)
    IV ptr_iv
    CODE:
        PerlUpb_DescriptorPool_DestroyRaw(aTHX_ INT2PTR(void*, ptr_iv));

SV*
_xs_generated_pool()
    CODE:
        RETVAL = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
    OUTPUT:
        RETVAL

SV*
_xs_add_serialized_file(self, serialized)
    SV* self
    SV* serialized
    CODE:
        RETVAL = PerlUpb_DescriptorPool_AddSerializedFile(aTHX_ self, serialized);
    OUTPUT:
        RETVAL

SV*
_xs_add_serialized_file_descriptor_set(self, serialized)
    SV* self
    SV* serialized
    CODE:
        RETVAL = PerlUpb_DescriptorPool_AddSerializedFileDescriptorSet(aTHX_ self, serialized);
    OUTPUT:
        RETVAL

SV*
_xs_find_file_by_name(self, name)
    SV* self
    const char* name
    CODE:
        RETVAL = PerlUpb_DescriptorPool_FindFileByName(aTHX_ self, name);
    OUTPUT:
        RETVAL

SV*
_xs_find_message_by_name(self, name)
    SV* self
    const char* name
    CODE:
        RETVAL = PerlUpb_DescriptorPool_FindMessageByName(aTHX_ self, name);
    OUTPUT:
        RETVAL

SV*
_xs_find_enum_by_name(self, name)
    SV* self
    const char* name
    CODE:
        RETVAL = PerlUpb_DescriptorPool_FindEnumByName(aTHX_ self, name);
    OUTPUT:
        RETVAL

SV*
_xs_find_extension_by_name(self, name)
    SV* self
    const char* name
    CODE:
        RETVAL = PerlUpb_DescriptorPool_FindExtensionByName(aTHX_ self, name);
    OUTPUT:
        RETVAL

