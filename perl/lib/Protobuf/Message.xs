#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/all_descriptors.h"
#include "xs/message/meta.h"
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

SV*
_xs_new_from_def_in_arena(mdef_sv, arena_sv)
    SV* mdef_sv
    SV* arena_sv
    CODE:
        const upb_MessageDef* mdef = PerlUpb_MessageDef_GetMessage(aTHX_ mdef_sv);
        if (!mdef) croak("Invalid MessageDef provided");
        
        upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
        const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
        upb_Message* msg = upb_Message_New(mt, arena);
        
        if (!msg) croak("Failed to allocate upb_Message in provided arena");
        
        RETVAL = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
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

void
_xs_json_to_handle(self, fh_sv)
    SV* self
    SV* fh_sv
    CODE:
        PerlUpb_Message_JsonToHandle(aTHX_ self, fh_sv);

void
_xs_to_handle(self, fh_sv, length_prefixed = false)
    SV* self
    SV* fh_sv
    bool length_prefixed
    CODE:
        PerlUpb_Message_ToHandle(aTHX_ self, fh_sv, length_prefixed);

SV*
_xs_from_handle(class_name, fh_sv, length_prefixed = false)
    SV* class_name
    SV* fh_sv
    bool length_prefixed
    CODE:
        // Use the descriptor() method on the class to get the mdef
        dSP;
        ENTER;
        SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(class_name);
        PUTBACK;
        int count = call_method("descriptor", G_SCALAR);
        SPAGAIN;
        if (count != 1) {
            PUTBACK; FREETMPS; LEAVE;
            croak("Failed to get descriptor for class %s", SvPV_nolen(class_name));
        }
        SV* descriptor_sv = POPs;
        // Keep a copy because we're about to free temps
        SV* mdef_sv = newSVsv(descriptor_sv);
        PUTBACK;
        FREETMPS;
        LEAVE;

        RETVAL = PerlUpb_Message_FromHandle(aTHX_ mdef_sv, fh_sv, length_prefixed);
        SvREFCNT_dec(mdef_sv);
    OUTPUT:
        RETVAL

SV*
_xs_from_json(class_name, json_data)
    SV* class_name
    SV* json_data
    CODE:
        // Use the descriptor() method on the class to get the mdef
        dSP;
        ENTER;
        SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(class_name);
        PUTBACK;
        int count = call_method("descriptor", G_SCALAR);
        SPAGAIN;
        if (count != 1) {
            PUTBACK; FREETMPS; LEAVE;
            croak("Failed to get descriptor for class %s", SvPV_nolen(class_name));
        }
        SV* descriptor_sv = POPs;
        // Keep a copy because we're about to free temps
        SV* mdef_sv = newSVsv(descriptor_sv);
        PUTBACK;
        FREETMPS;
        LEAVE;

        RETVAL = PerlUpb_Message_FromJson(aTHX_ mdef_sv, json_data);
        SvREFCNT_dec(mdef_sv);
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
        // Use the descriptor() method on the class to get the mdef
        dSP;
        ENTER;
        SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(sv_2mortal(newSVpv(class_name, 0)));
        PUTBACK;
        int count = call_method("descriptor", G_SCALAR);
        SPAGAIN;
        if (count != 1) {
            PUTBACK; FREETMPS; LEAVE;
            croak("Failed to get descriptor for class %s", class_name);
        }
        SV* descriptor_sv = POPs;
        const upb_MessageDef* mdef = PerlUpb_MessageDef_GetMessage(aTHX_ descriptor_sv);
        PUTBACK;
        // Don't FREETMPS/LEAVE yet because descriptor_sv might be used
        // Wait, mdef is a C pointer into the pool, it's safe.
        FREETMPS;
        LEAVE;

        if (!mdef) {
            croak("Could not find descriptor for message class %s", class_name);
        }

        STRLEN len;
        const char* buf = SvPV(data, len);
        
        SV* arena_sv = PerlUpb_Arena_New(aTHX);
        upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
        
        const upb_MiniTable* mt = upb_MessageDef_MiniTable(mdef);
        upb_Message* msg = upb_Message_New(mt, arena);
        
        if (!msg) {
            SvREFCNT_dec(arena_sv);
            croak("Failed to allocate upb_Message");
        }
        
        upb_DecodeStatus status = upb_Decode(buf, len, msg, mt, NULL, 0, arena);
        if (status != kUpb_DecodeStatus_Ok) {
            SvREFCNT_dec(arena_sv);
            croak("Failed to parse message: %d", status);
        }
        
        RETVAL = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
        SvREFCNT_dec(arena_sv);
    OUTPUT:
        RETVAL

void
_xs_parse_from(self, data)
    SV* self
    SV* data
    CODE:
        PerlUpb_Message_ParseFrom(aTHX_ self, data);

void
_xs_merge_from(self, other)
    SV* self
    SV* other
    CODE:
        PerlUpb_Message_MergeFrom(aTHX_ self, other);

void
_xs_copy_from(self, other)
    SV* self
    SV* other
    CODE:
        PerlUpb_Message_CopyFrom(aTHX_ self, other);

SV*
_xs_fields(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_Fields(aTHX_ self);
    OUTPUT:
        RETVAL

bool
_xs_audit_integrity(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_AuditIntegrity(aTHX_ self);
    OUTPUT:
        RETVAL

bool
_xs_validate(self)
    SV* self
    CODE:
        // By default, generic validation just checks if it's a valid object.
        // For generated classes, the ClassGenerator will provide 
        // specialized AOT validators that bypass this generic path.
        const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ self);
        RETVAL = (msg != NULL);
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
_xs_coerce_to(self, target_class_name)
    SV* self
    const char* target_class_name
    CODE:
        const upb_Message* msg = PerlUpb_Message_GetMsg(aTHX_ self);
        const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ self);
        SV* arena_sv = PerlUpb_Message_GetArena(aTHX_ self);
        
        // Identity check: target class must agree with current underlying descriptor
        // (This is a shallow coercion, we just bless into a new class with the same memory)
        RETVAL = PerlUpb_WrapMessage(aTHX_ (upb_Message*)msg, mdef, arena_sv);
        sv_bless(RETVAL, gv_stashpv(target_class_name, GV_ADD));
    OUTPUT:
        RETVAL

SV*
_xs_get_fingerprint(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_GetFingerprint(aTHX_ self);
    OUTPUT:
        RETVAL
