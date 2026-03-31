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
#include "xs/unknown_fields/set.h"
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

// -- Message --
MODULE = Protobuf::Message  PACKAGE = Protobuf::Message
PROTOTYPES: ENABLE


SV*
_xs_new_from_def(mdef_sv)
    SV* mdef_sv
    CODE:
        const upb_MessageDef* mdef = PerlUpb_MessageDef_GetMessage(aTHX_ mdef_sv);
        
        if (!mdef) {
            croak("Invalid MessageDef provided to new()");
        }
        
        RETVAL = PerlUpb_Message_NewMessage(aTHX_ mdef_sv);
    OUTPUT:
        RETVAL

void
_xs_free(self)
    SV* self
    CODE:
        PerlUpb_Message_Free(aTHX_ self);

SV*
_xs_get(self, field_name)
    SV* self
    const char* field_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        const upb_FieldDef* fdef = PerlUpb_MessageDef_FindFieldByName(aTHX_ mdef, field_name);
        if (!fdef) {
            croak("Field '%s' not found in message '%s'", field_name, upb_MessageDef_FullName(mdef));
        }
        RETVAL = PerlUpb_Message_GetField(aTHX_ self, fdef);
    OUTPUT:
        RETVAL

void
_xs_set(self, field_name, value)
    SV* self
    const char* field_name
    SV* value
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        const upb_FieldDef* fdef = PerlUpb_MessageDef_FindFieldByName(aTHX_ mdef, field_name);
        if (!fdef) {
            croak("Field '%s' not found in message '%s'", field_name, upb_MessageDef_FullName(mdef));
        }
        PerlUpb_Message_SetField(aTHX_ self, fdef, value);

bool
_xs_has(self, field_name)
    SV* self
    const char* field_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        const upb_FieldDef* fdef = PerlUpb_MessageDef_FindFieldByName(aTHX_ mdef, field_name);
        if (!fdef) {
            croak("Field '%s' not found in message '%s'", field_name, upb_MessageDef_FullName(mdef));
        }
        RETVAL = PerlUpb_Message_HasField(aTHX_ self, fdef);
    OUTPUT:
        RETVAL

void
_xs_clear(self, field_name)
    SV* self
    const char* field_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        const upb_FieldDef* fdef = PerlUpb_MessageDef_FindFieldByName(aTHX_ mdef, field_name);
        if (!fdef) {
            croak("Field '%s' not found in message '%s'", field_name, upb_MessageDef_FullName(mdef));
        }
        PerlUpb_Message_ClearField(aTHX_ self, fdef);

const char*
_xs_which_oneof(self, oneof_name)
    SV* self
    const char* oneof_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        const upb_OneofDef* odef = upb_MessageDef_FindOneofByName(mdef, oneof_name);
        if (!odef) {
            croak("Oneof '%s' not found in message '%s'", oneof_name, upb_MessageDef_FullName(mdef));
        }
        RETVAL = PerlUpb_Message_WhichOneof(aTHX_ self, odef);
    OUTPUT:
        RETVAL

SV*
_xs_serialize(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_Serialize(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_unknown_fields(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_UnknownFieldSet_New(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_parse(class_name, data)
    const char* class_name
    SV* data
    CODE:
        char* full_name = savepv(class_name);
        for (char* p = full_name; *p; p++) {
            if (*p == ':' && *(p+1) == ':') {
                *p = '.';
                memmove(p+1, p+2, strlen(p+2) + 1);
            }
        }
        
        SV* pool_sv = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
        const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ pool_sv);
        const upb_MessageDef* mdef = upb_DefPool_FindMessageByName(pool, full_name);
        Safefree(full_name);

        if (!mdef) {
            croak("Could not find descriptor for message class %s", class_name);
        }

        STRLEN len;
        const char* buf = SvPV(data, len);
        
        SV* arena_sv = PerlUpb_Arena_New(aTHX);
        upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
        
        const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
        upb_Message* msg = upb_Message_New(mt, arena);
        
        upb_DecodeStatus status = upb_Decode(buf, len, msg, mt, NULL, 0, arena);
        if (status != kUpb_DecodeStatus_Ok) {
            SvREFCNT_dec(arena_sv);
            croak("Failed to parse message: %d", status);
        }
        
        RETVAL = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
        SvREFCNT_dec(arena_sv);
    OUTPUT:
        RETVAL
