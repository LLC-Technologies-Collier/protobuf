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

// -- Descriptor::File --
MODULE = Protobuf::Descriptor::File  PACKAGE = Protobuf::Descriptor::File
PROTOTYPES: ENABLE


const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_FileDef* file = PerlUpb_FileDef_GetFile(aTHX_ self);
        RETVAL = file ? upb_FileDef_Name(file) : NULL;
    OUTPUT:
        RETVAL

const char*
_xs_package(self)
    SV* self
    CODE:
        const upb_FileDef* file = PerlUpb_FileDef_GetFile(aTHX_ self);
        RETVAL = file ? upb_FileDef_Package(file) : NULL;
    OUTPUT:
        RETVAL

int
_xs_top_level_message_count(self)
    SV* self
    CODE:
        const upb_FileDef* file = PerlUpb_FileDef_GetFile(aTHX_ self);
        RETVAL = file ? upb_FileDef_TopLevelMessageCount(file) : 0;
    OUTPUT:
        RETVAL

SV*
_xs_top_level_message(self, index)
    SV* self
    int index
    CODE:
        const upb_FileDef* file = PerlUpb_FileDef_GetFile(aTHX_ self);
        if (file && index >= 0 && index < upb_FileDef_TopLevelMessageCount(file)) {
            RETVAL = PerlUpb_MessageDef_GetWrapper(aTHX_ upb_FileDef_TopLevelMessage(file, index));
        } else {
            RETVAL = &PL_sv_undef;
        }
    OUTPUT:
        RETVAL

