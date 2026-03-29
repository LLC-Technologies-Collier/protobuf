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

MODULE = Protobuf  PACKAGE = Protobuf

# -- Arena --
MODULE = Protobuf  PACKAGE = Protobuf::Arena

IV
_xs_create_raw()
    CODE:
        RETVAL = PTR2IV(PerlUpb_Arena_CreateRaw(aTHX));
    OUTPUT:
        RETVAL

void
_xs_destroy_raw(ptr_iv)
    IV ptr_iv
    CODE:
        PerlUpb_Arena_DestroyRaw(aTHX_ INT2PTR(void*, ptr_iv));

# -- DescriptorPool --
MODULE = Protobuf  PACKAGE = Protobuf::DescriptorPool

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

# -- Descriptor::File --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::File

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

# -- Descriptor::MessageDef --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::MessageDef

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

# -- Descriptor::Enum --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::Enum

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

# -- Descriptor::Field --
MODULE = Protobuf  PACKAGE = Protobuf::Descriptor::Field

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

# -- Message --
MODULE = Protobuf  PACKAGE = Protobuf::Message

SV*
_xs_new_from_class(class_name)
    const char* class_name
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
        
        if (!mdef) {
            Safefree(full_name);
            croak("Could not find descriptor for message class %s", class_name);
        }
        
        RETVAL = PerlUpb_Message_NewMessage(aTHX_ PerlUpb_MessageDef_GetWrapper(aTHX_ mdef));
        Safefree(full_name);
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

SV*
_xs_serialize(self)
    SV* self
    CODE:
        RETVAL = PerlUpb_Message_Serialize(aTHX_ self);
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
