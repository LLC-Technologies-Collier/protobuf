#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/message.h"
#include "t/c/convert/test_util.h" // For load_test_descriptors
#include "upb/reflection/def.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(2 + 16); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    ok(msg_def != NULL, "Found test.TestMessage");

    if (msg_def) {
        is_string(PerlUpb_MessageDef_FullName(aTHX_ msg_def), "test.TestMessage", "PerlUpb_MessageDef_FullName");
        is_string(PerlUpb_MessageDef_Name(aTHX_ msg_def), "TestMessage", "PerlUpb_MessageDef_Name");
        
        ok(PerlUpb_MessageDef_FieldCount(aTHX_ msg_def) > 0, "PerlUpb_MessageDef_FieldCount");
        
        const upb_FieldDef *f1 = PerlUpb_MessageDef_FindFieldByNumber(aTHX_ msg_def, 1);
        ok(f1 != NULL, "PerlUpb_MessageDef_FindFieldByNumber(1)");
        is_string(upb_FieldDef_Name(f1), "value", "Field 1 name");

        const upb_FieldDef *f_val = PerlUpb_MessageDef_FindFieldByName(aTHX_ msg_def, "value");
        ok(f_val == f1, "PerlUpb_MessageDef_FindFieldByName('value')");

        ok(PerlUpb_MessageDef_OneofCount(aTHX_ msg_def) == 1, "PerlUpb_MessageDef_OneofCount");
        ok(PerlUpb_MessageDef_RealOneofCount(aTHX_ msg_def) == 1, "PerlUpb_MessageDef_RealOneofCount");

        // Nested messages and enums
        ok(PerlUpb_MessageDef_NestedMessageCount(aTHX_ msg_def) == 1, "PerlUpb_MessageDef_NestedMessageCount"); // OptionalGroup is a group/message
        ok(PerlUpb_MessageDef_NestedEnumCount(aTHX_ msg_def) == 1, "PerlUpb_MessageDef_NestedEnumCount");
        
        ok(PerlUpb_MessageDef_File(aTHX_ msg_def) != NULL, "PerlUpb_MessageDef_File");
        ok(PerlUpb_MessageDef_ContainingType(aTHX_ msg_def) == NULL, "PerlUpb_MessageDef_ContainingType (top-level)");

        ok(PerlUpb_MessageDef_Field(aTHX_ msg_def, 0) != NULL, "PerlUpb_MessageDef_Field(0)");
        ok(PerlUpb_MessageDef_Oneof(aTHX_ msg_def, 0) != NULL, "PerlUpb_MessageDef_Oneof(0)");
        
        const upb_MessageDef *nested_msg = PerlUpb_MessageDef_NestedMessage(aTHX_ msg_def, 0);
        ok(nested_msg != NULL, "PerlUpb_MessageDef_NestedMessage(0)");
        
        const upb_EnumDef *nested_enum = PerlUpb_MessageDef_NestedEnum(aTHX_ msg_def, 0);
        ok(nested_enum != NULL, "PerlUpb_MessageDef_NestedEnum(0)");

        ok(!PerlUpb_MessageDef_IsMapEntry(aTHX_ msg_def), "PerlUpb_MessageDef_IsMapEntry");
        ok(!PerlUpb_MessageDef_IsMessageSet(aTHX_ msg_def), "PerlUpb_MessageDef_IsMessageSet");

    } else {
        cdiag("Skipping message tests as msg_def is NULL");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
