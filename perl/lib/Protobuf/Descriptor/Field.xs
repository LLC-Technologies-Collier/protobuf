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

// -- Descriptor::Field --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::Field
PROTOTYPES: ENABLE


const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_Name(f) : NULL;
    OUTPUT:
        RETVAL

const char*
_xs_full_name(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_FullName(f) : NULL;
    OUTPUT:
        RETVAL

uint32_t
_xs_number(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_Number(f) : 0;
    OUTPUT:
        RETVAL

int
_xs_type(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_Type(f) : 0;
    OUTPUT:
        RETVAL

int
_xs_label(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_Label(f) : 0;
    OUTPUT:
        RETVAL

bool
_xs_is_repeated(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_IsRepeated(f) : false;
    OUTPUT:
        RETVAL

bool
_xs_is_map(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        RETVAL = f ? upb_FieldDef_IsMap(f) : false;
    OUTPUT:
        RETVAL

SV*
_xs_message_type(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        const upb_MessageDef* m = f ? upb_FieldDef_MessageSubDef(f) : NULL;
        RETVAL = m ? PerlUpb_MessageDef_GetWrapper(aTHX_ m) : &PL_sv_undef;
    OUTPUT:
        RETVAL

SV*
_xs_enum_type(self)
    SV* self
    CODE:
        const upb_FieldDef* f = PerlUpb_FieldDef_GetField(aTHX_ self);
        const upb_EnumDef* e = f ? upb_FieldDef_EnumSubDef(f) : NULL;
        RETVAL = e ? PerlUpb_EnumDef_GetWrapper(aTHX_ e) : &PL_sv_undef;
    OUTPUT:
        RETVAL

