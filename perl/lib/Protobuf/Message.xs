#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/all_descriptors.h"
#include "xs/protobuf/utils.h"
#include "xs/unknown_fields/set.h"

// -- Message --
#define GET_FIELD_DEF_OR_CROAK(mdef, field_name, fdef_var) \
    const upb_FieldDef* fdef_var = upb_MessageDef_FindFieldByName(mdef, field_name); \
    if (!fdef_var) { \
        croak("Field '%s' not found in message '%s'", field_name, upb_MessageDef_FullName(mdef)); \
    }

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
        GET_FIELD_DEF_OR_CROAK(mdef, field_name, fdef);
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
        GET_FIELD_DEF_OR_CROAK(mdef, field_name, fdef);
        PerlUpb_Message_SetField(aTHX_ self, fdef, value);

bool
_xs_has(self, field_name)
    SV* self
    const char* field_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        GET_FIELD_DEF_OR_CROAK(mdef, field_name, fdef);
        RETVAL = PerlUpb_Message_HasField(aTHX_ self, fdef);
    OUTPUT:
        RETVAL

void
_xs_clear(self, field_name)
    SV* self
    const char* field_name
    CODE:
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        GET_FIELD_DEF_OR_CROAK(mdef, field_name, fdef);
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
_xs_get_by_fdef(self, fdef_sv)
    SV* self
    SV* fdef_sv
    CODE:
        const upb_FieldDef* fdef = PerlUpb_FieldDef_GetField(aTHX_ fdef_sv);
        if (!fdef) croak("Invalid FieldDef provided to _xs_get_by_fdef");
        RETVAL = PerlUpb_Message_GetField(aTHX_ self, fdef);
    OUTPUT:
        RETVAL

void
_xs_set_by_fdef(self, fdef_sv, value)
    SV* self
    SV* fdef_sv
    SV* value
    CODE:
        const upb_FieldDef* fdef = PerlUpb_FieldDef_GetField(aTHX_ fdef_sv);
        if (!fdef) croak("Invalid FieldDef provided to _xs_set_by_fdef");
        PerlUpb_Message_SetField(aTHX_ self, fdef, value);

bool
_xs_has_by_fdef(self, fdef_sv)
    SV* self
    SV* fdef_sv
    CODE:
        const upb_FieldDef* fdef = PerlUpb_FieldDef_GetField(aTHX_ fdef_sv);
        if (!fdef) croak("Invalid FieldDef provided to _xs_has_by_fdef");
        RETVAL = PerlUpb_Message_HasField(aTHX_ self, fdef);
    OUTPUT:
        RETVAL

void
_xs_clear_by_fdef(self, fdef_sv)
    SV* self
    SV* fdef_sv
    CODE:
        const upb_FieldDef* fdef = PerlUpb_FieldDef_GetField(aTHX_ fdef_sv);
        if (!fdef) croak("Invalid FieldDef provided to _xs_clear_by_fdef");
        PerlUpb_Message_ClearField(aTHX_ self, fdef);

SV*
_xs_serialize(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_Serialize(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_to_perl(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_ToPerl(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_to_text(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_ToText(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_to_json(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_ToJson(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
_xs_from_json(class_name, json_data)
    SV* class_name
    SV* json_data
    CODE:
        RETVAL = PerlUpb_Message_FromJson(aTHX_ class_name, json_data);
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
        char* full_name = PerlUpb_ClassNameToFullName(aTHX_ class_name);
        
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

SV*
_xs_migrate_to_arena(self, arena_sv)
    SV* self
    SV* arena_sv
    CODE:
        const upb_Message* src_msg = PerlUpb_Message_GetMsg(aTHX_ self);
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
        
        const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
        upb_Message* dst_msg = upb_Message_New(mt, arena);
        
        /*
        if (!upb_Message_DeepCopy(dst_msg, src_msg, mt, arena)) {
            croak("Failed to migrate message to shared arena");
        }
        */
        croak("Message migration requires upb_Message_DeepCopy (not available in current upb)");
        
        RETVAL = PerlUpb_WrapMessage(aTHX_ dst_msg, mdef, arena_sv);
    OUTPUT:
        RETVAL

SV*
_xs_find_in_shared_arena(class_name, arena_sv)
    const char* class_name
    SV* arena_sv
    CODE:
        // Implementation detail: for this high-level handoff, 
        // we'll assume the message is at a fixed offset (after canaries) 
        // or we need a tracking header in the arena. 
        // For the purpose of the goal, we'll try to find it.
        // Actually, we can't "find" it without metadata.
        // A better approach is to store the message pointer IV in a file next to the tmpfs.
        // For now, this is a placeholder for the reification logic.
        croak("find_in_shared_arena requires message metadata tracking (TODO)");
    OUTPUT:
        RETVAL
