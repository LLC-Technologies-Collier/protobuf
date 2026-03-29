#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/field.h"
#include "t/c/convert/test_util.h" // For load_test_descriptors
#include "upb/reflection/def.h"
#include "upb/base/descriptor_constants.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(2 + 15); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    ok(msg_def, "Found test.TestMessage");

    if (msg_def) {
        const upb_FieldDef* field = upb_MessageDef_FindFieldByName(msg_def, "optional_int32");
        ok(field, "Found field optional_int32");

        if (field) {
            is_string(PerlUpb_FieldDef_Name(aTHX_ field), "optional_int32", "PerlUpb_FieldDef_Name");
            is_string(PerlUpb_FieldDef_FullName(aTHX_ field), "test.TestMessage.optional_int32", "PerlUpb_FieldDef_FullName");
            is(PerlUpb_FieldDef_Index(aTHX_ field), 0, "PerlUpb_FieldDef_Index");
            is(PerlUpb_FieldDef_Type(aTHX_ field), kUpb_FieldType_Int32, "PerlUpb_FieldDef_Type");
            is(PerlUpb_FieldDef_Label(aTHX_ field), kUpb_Label_Optional, "PerlUpb_FieldDef_Label");
            is(PerlUpb_FieldDef_Number(aTHX_ field), 1, "PerlUpb_FieldDef_Number");
            ok(!PerlUpb_FieldDef_IsRequired(aTHX_ field), "PerlUpb_FieldDef_IsRequired");
            ok(PerlUpb_FieldDef_IsOptional(aTHX_ field), "PerlUpb_FieldDef_IsOptional");
            ok(!PerlUpb_FieldDef_IsRepeated(aTHX_ field), "PerlUpb_FieldDef_IsRepeated");
            ok(!PerlUpb_FieldDef_IsPacked(aTHX_ field), "PerlUpb_FieldDef_IsPacked");
            ok(!PerlUpb_FieldDef_IsMap(aTHX_ field), "PerlUpb_FieldDef_IsMap");
            ok(PerlUpb_FieldDef_ContainingType(aTHX_ field) == msg_def, "PerlUpb_FieldDef_ContainingType");
            ok(PerlUpb_FieldDef_ContainingOneof(aTHX_ field) == NULL, "PerlUpb_FieldDef_ContainingOneof");
            ok(PerlUpb_FieldDef_MessageSubDef(aTHX_ field) == NULL, "PerlUpb_FieldDef_MessageSubDef");
            ok(PerlUpb_FieldDef_EnumSubDef(aTHX_ field) == NULL, "PerlUpb_FieldDef_EnumSubDef");
            ok(PerlUpb_FieldDef_HasPresence(aTHX_ field), "PerlUpb_FieldDef_HasPresence");
        } else {
            fprintf(stderr, "# Skipping FieldDef tests as field is NULL\n");
        }
    } else {
        fprintf(stderr, "# Skipping FieldDef tests as msg_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}