#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/descriptor/oneof.h"
#include "xs/descriptor/field.h"

MODULE = Protobuf::Descriptor::OneofDef  PACKAGE = Protobuf::Descriptor::OneofDef
PROTOTYPES: ENABLE

const char*
_xs_full_name(self)
    SV* self
    CODE:
        const upb_OneofDef* o = PerlUpb_OneofDef_GetOneof(aTHX_ self);
        RETVAL = o ? PerlUpb_OneofDef_FullName(aTHX_ o) : NULL;
    OUTPUT:
        RETVAL

const char*
_xs_name(self)
    SV* self
    CODE:
        const upb_OneofDef* o = PerlUpb_OneofDef_GetOneof(aTHX_ self);
        RETVAL = o ? PerlUpb_OneofDef_Name(aTHX_ o) : NULL;
    OUTPUT:
        RETVAL

int
_xs_field_count(self)
    SV* self
    CODE:
        const upb_OneofDef* o = PerlUpb_OneofDef_GetOneof(aTHX_ self);
        RETVAL = o ? PerlUpb_OneofDef_FieldCount(aTHX_ o) : 0;
    OUTPUT:
        RETVAL

SV*
_xs_field(self, index)
    SV* self
    int index
    CODE:
        const upb_OneofDef* o = PerlUpb_OneofDef_GetOneof(aTHX_ self);
        if (o && index >= 0 && index < PerlUpb_OneofDef_FieldCount(aTHX_ o)) {
            RETVAL = PerlUpb_FieldDef_GetWrapper(aTHX_ PerlUpb_OneofDef_Field(aTHX_ o, index));
        } else {
            RETVAL = &PL_sv_undef;
        }
    OUTPUT:
        RETVAL

bool
_xs_is_synthetic(self)
    SV* self
    CODE:
        const upb_OneofDef* o = PerlUpb_OneofDef_GetOneof(aTHX_ self);
        RETVAL = o ? PerlUpb_OneofDef_IsSynthetic(aTHX_ o) : false;
    OUTPUT:
        RETVAL
