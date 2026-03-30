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

// -- Descriptor::MessageDef --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::MessageDef
PROTOTYPES: ENABLE


const char*
_xs_full_name(self)
    SV* self
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        RETVAL = m ? upb_MessageDef_FullName(m) : NULL;
    OUTPUT:
        RETVAL

const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        RETVAL = m ? upb_MessageDef_Name(m) : NULL;
    OUTPUT:
        RETVAL

int
_xs_field_count(self)
    SV* self
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        RETVAL = m ? upb_MessageDef_FieldCount(m) : 0;
    OUTPUT:
        RETVAL

SV*
_xs_field(self, index)
    SV* self
    int index
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        if (m && index >= 0 && index < upb_MessageDef_FieldCount(m)) {
            RETVAL = PerlUpb_FieldDef_GetWrapper(aTHX_ upb_MessageDef_Field(m, index));
        } else {
            RETVAL = &PL_sv_undef;
        }
    OUTPUT:
        RETVAL

SV*
_xs_find_field_by_name(self, name)
    SV* self
    SV* name
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        STRLEN len;
        const char* name_ptr = SvPV(name, len);
        const upb_FieldDef* f = m ? PerlUpb_MessageDef_FindFieldByNameWithSize(aTHX_ m, name_ptr, len) : NULL;
        RETVAL = f ? PerlUpb_FieldDef_GetWrapper(aTHX_ f) : &PL_sv_undef;
    OUTPUT:
        RETVAL

SV*
_xs_find_field_by_number(self, number)
    SV* self
    int number
    CODE:
        const upb_MessageDef* m = PerlUpb_MessageDef_GetMessage(aTHX_ self);
        const upb_FieldDef* f = m ? PerlUpb_MessageDef_FindFieldByNumber(aTHX_ m, number) : NULL;
        RETVAL = f ? PerlUpb_FieldDef_GetWrapper(aTHX_ f) : &PL_sv_undef;
    OUTPUT:
        RETVAL

