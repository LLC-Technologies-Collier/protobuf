#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor_containers/by_name_map.h"
#include "xs/descriptor_containers/generic_sequence.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "t/c/convert/test_util.h"
#include "upb/reflection/def.h"
#include <stdio.h>

// VTable for MessageDef.fields (Sequence)
int msg_field_count(const void* p) { return upb_MessageDef_FieldCount((const upb_MessageDef*)p); }
const void* msg_field_get(const void* p, int i) { return upb_MessageDef_Field((const upb_MessageDef*)p, i); }
SV* msg_field_wrap(pTHX_ const void* p) {
    // In the real implementation, this would use the object cache and return a Protobuf::Descriptor::Field.
    // For this test, we just return a simple blessed wrapper or even just an IV.
    SV* sv = newSViv((IV)p);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::Descriptor::Field", GV_ADD));
    return obj;
}

static const PerlUpb_GenericSequence_VTable msg_fields_seq_vtable = {
    msg_field_count, msg_field_get, msg_field_wrap
};

// VTable for MessageDef.fields_by_name (ByNameMap)
const void* msg_field_lookup(const void* p, const char* n) { return upb_MessageDef_FindFieldByName((const upb_MessageDef*)p, n); }
const char* msg_field_key(const void* p, int i) {
    const upb_FieldDef* f = upb_MessageDef_Field((const upb_MessageDef*)p, i);
    return f ? upb_FieldDef_Name(f) : NULL;
}
const void* msg_field_value(const void* p, int i) { return upb_MessageDef_Field((const upb_MessageDef*)p, i); }

static const PerlUpb_ByNameMap_VTable msg_fields_map_vtable = {
    msg_field_count, msg_field_lookup, msg_field_key, msg_field_value, msg_field_wrap
};

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(11);

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    ok(msg_def != NULL, "Found test.TestMessage");

    if (msg_def) {
        SV* parent_sv = newSViv(1); // Fake parent

        // 1. Test Sequence (fields)
        SV* seq_sv = PerlUpb_GenericSequence_New(aTHX_ parent_sv, msg_def, &msg_fields_seq_vtable);
        ok(seq_sv != NULL, "Created fields sequence");
        int count = PerlUpb_GenericSequence_Count(aTHX_ seq_sv);
        ok(count > 0, "Field count > 0");
        
        SV* f0 = PerlUpb_GenericSequence_GetItem(aTHX_ seq_sv, 0);
        ok(sv_derived_from(f0, "Protobuf::Descriptor::Field"), "Item 0 is a FieldDescriptor");
        SvREFCNT_dec(f0);

        // 2. Test ByNameMap (fields_by_name)
        SV* map_sv = PerlUpb_ByNameMap_New(aTHX_ parent_sv, msg_def, &msg_fields_map_vtable);
        ok(map_sv != NULL, "Created fields_by_name map");
        is(PerlUpb_ByNameMap_Count(aTHX_ map_sv), count, "Map count matches sequence count");
        
        SV* val_sv = PerlUpb_ByNameMap_Lookup(aTHX_ map_sv, "value");
        ok(sv_derived_from(val_sv, "Protobuf::Descriptor::Field"), "Lookup 'value' returned a FieldDescriptor");
        
        const upb_FieldDef* f_raw = (const upb_FieldDef*)SvIV(SvRV(val_sv));
        is_string(upb_FieldDef_Name(f_raw), "value", "Raw field name is 'value'");
        
        SvREFCNT_dec(val_sv);

        SV* key0 = PerlUpb_ByNameMap_Key(aTHX_ map_sv, 0);
        ok(SvPOK(key0), "Key 0 is a string");
        cdiag("Key 0 name: %s", SvPV_nolen(key0));
        SvREFCNT_dec(key0);

        SV* val0 = PerlUpb_ByNameMap_Value(aTHX_ map_sv, 0);
        ok(sv_derived_from(val0, "Protobuf::Descriptor::Field"), "Value 0 is a FieldDescriptor");
        SvREFCNT_dec(val0);

        extern void PerlUpb_ByNameMap_Free(pTHX_ SV* sv);
        PerlUpb_ByNameMap_Free(aTHX_ map_sv);
        extern void PerlUpb_GenericSequence_Free(pTHX_ SV* sv);
        PerlUpb_GenericSequence_Free(aTHX_ seq_sv);

        SvREFCNT_dec(seq_sv);
        SvREFCNT_dec(map_sv);
        SvREFCNT_dec(parent_sv);
    } else {
        fprintf(stderr, "# Skipping integration tests as msg_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
