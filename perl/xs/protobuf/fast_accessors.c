#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/protobuf/message.h"
#include "xs/message/access.h"
#include "xs/descriptor/field.h"
#include "upb/reflection/def.h"

// The fast accessor XSUB implementation
static XS(PerlUpb_FastAccessor) {
    dXSARGS;
    
    // The field def pointer is stored in the CV's ANY pointer
    // 'cv' is automatically provided as a parameter to the XS function
    const upb_FieldDef* f = (const upb_FieldDef*)CvXSUBANY(cv).any_ptr;
    
    if (items < 1) croak("Usage: $msg->field([value])");
    SV* self = ST(0);

    if (items > 1) {
        // Setter mode
        PerlUpb_Message_SetField(aTHX_ self, f, ST(1));
        XSRETURN_EMPTY;
    } else {
        // Getter mode
        SV* result = PerlUpb_Message_GetField(aTHX_ self, f);
        ST(0) = result;
        // Result is already mortal if it came from PerlUpb_UpbToSv
        XSRETURN(1);
    }
}

// Specialized setter for performance
static XS(PerlUpb_FastSetter) {
    dXSARGS;
    const upb_FieldDef* f = (const upb_FieldDef*)CvXSUBANY(cv).any_ptr;
    if (items != 2) croak("Usage: $msg->set_field(value)");
    PerlUpb_Message_SetField(aTHX_ ST(0), f, ST(1));
    XSRETURN_EMPTY;
}

// Specialized has_field
static XS(PerlUpb_FastHas) {
    dXSARGS;
    const upb_FieldDef* f = (const upb_FieldDef*)CvXSUBANY(cv).any_ptr;
    if (items != 1) croak("Usage: $msg->has_field()");
    bool has = PerlUpb_Message_HasField(aTHX_ ST(0), f);
    ST(0) = has ? &PL_sv_yes : &PL_sv_no;
    XSRETURN(1);
}

// Specialized clear_field
static XS(PerlUpb_FastClear) {
    dXSARGS;
    const upb_FieldDef* f = (const upb_FieldDef*)CvXSUBANY(cv).any_ptr;
    if (items != 1) croak("Usage: $msg->clear_field()");
    PerlUpb_Message_ClearField(aTHX_ ST(0), f);
    XSRETURN_EMPTY;
}

void PerlUpb_InstallFastAccessors(pTHX_ const char* perl_class, const upb_MessageDef* mdef) {
    int field_count = upb_MessageDef_FieldCount(mdef);
    
    for (int i = 0; i < field_count; i++) {
        const upb_FieldDef* f = upb_MessageDef_Field(mdef, i);
        const char* name = upb_FieldDef_Name(f);
        
        // 1. Install primary accessor (getter/setter)
        {
            char full_name[512];
            snprintf(full_name, sizeof(full_name), "%s::%s", perl_class, name);
            CV* cv = newXS(full_name, PerlUpb_FastAccessor, __FILE__);
            CvXSUBANY(cv).any_ptr = (void*)f;
        }

        // 2. Install explicit setter
        {
            char full_name[512];
            snprintf(full_name, sizeof(full_name), "%s::set_%s", perl_class, name);
            CV* cv = newXS(full_name, PerlUpb_FastSetter, __FILE__);
            CvXSUBANY(cv).any_ptr = (void*)f;
        }

        // 3. Install has_ (if applicable)
        if (upb_FieldDef_HasPresence(f)) {
            char full_name[512];
            snprintf(full_name, sizeof(full_name), "%s::has_%s", perl_class, name);
            CV* cv = newXS(full_name, PerlUpb_FastHas, __FILE__);
            CvXSUBANY(cv).any_ptr = (void*)f;
        }

        // 4. Install clear_
        {
            char full_name[512];
            snprintf(full_name, sizeof(full_name), "%s::clear_%s", perl_class, name);
            CV* cv = newXS(full_name, PerlUpb_FastClear, __FILE__);
            CvXSUBANY(cv).any_ptr = (void*)f;
        }
    }
}
