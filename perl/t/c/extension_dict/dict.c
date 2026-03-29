#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/extension_dict/dict.h"
#include "xs/extension_dict/iterator.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(9);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    const upb_FieldDef *ext_field = upb_DefPool_FindExtensionByName(test_pool, "test.extension_string");
    ok(mdef != NULL, "Found test.TestMessage");
    ok(ext_field != NULL, "Found test.extension_string");

    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    SV* message_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
    ok(message_sv != NULL, "Wrapped message");
    
    SV* dict_sv = PerlUpb_ExtensionDict_New(aTHX_ message_sv);
    ok(dict_sv != NULL, "Created ExtensionDict");

    // Test SetItem
    SV* ext_field_sv = PerlUpb_FieldDef_GetWrapper(aTHX_ ext_field);
    SV* val_sv = newSVpv("hello extension", 0);
    PerlUpb_ExtensionDict_SetItem(aTHX_ dict_sv, ext_field_sv, val_sv);
    ok(1, "Set extension value");

    // Test GetItem
    SV* retrieved_val = PerlUpb_ExtensionDict_GetItem(aTHX_ dict_sv, ext_field_sv);
    is_string(SvPV_nolen(retrieved_val), "hello extension", "Retrieved extension value matches");
    SvREFCNT_dec(retrieved_val);

    // Test Iterator
    SV* iter_sv = PerlUpb_ExtensionDict_GetIterator(aTHX_ dict_sv);
    SV* next_f = PerlUpb_ExtensionDict_Iterator_Next(aTHX_ iter_sv);
    ok(next_f != NULL && sv_derived_from(next_f, "Protobuf::Descriptor::Field"), "Iterator returns field");
    
    const upb_FieldDef* next_f_raw = PerlUpb_FieldDef_GetField(aTHX_ next_f);
    is(next_f_raw, ext_field, "Iterator returned correct field");

    // Cleanup
    SvREFCNT_dec(next_f);
    
    extern void PerlUpb_ExtensionDictIterator_Free(pTHX_ SV* sv);
    PerlUpb_ExtensionDictIterator_Free(aTHX_ iter_sv);
    SvREFCNT_dec(iter_sv);
    
    SvREFCNT_dec(ext_field_sv);
    SvREFCNT_dec(val_sv);

    extern void PerlUpb_ExtensionDict_Free(pTHX_ SV* sv);
    PerlUpb_ExtensionDict_Free(aTHX_ dict_sv);
    SvREFCNT_dec(dict_sv);
    
    SvREFCNT_dec(message_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
