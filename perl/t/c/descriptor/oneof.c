#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/oneof.h"
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

    plan(2 + 10); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    ok(msg_def != NULL, "Found test.TestMessage");

    if (msg_def) {
        const upb_OneofDef *oneof_def = upb_MessageDef_Oneof(msg_def, 0);
        ok(oneof_def != NULL, "Found Oneof 0");

        if (oneof_def) {
            is_string(PerlUpb_OneofDef_FullName(aTHX_ oneof_def), "test.TestMessage.test_oneof", "PerlUpb_OneofDef_FullName");
            is_string(PerlUpb_OneofDef_Name(aTHX_ oneof_def), "test_oneof", "PerlUpb_OneofDef_Name");
            is(PerlUpb_OneofDef_FieldCount(aTHX_ oneof_def), 2, "PerlUpb_OneofDef_FieldCount");
            
            const upb_MessageDef *containing_type = PerlUpb_OneofDef_ContainingType(aTHX_ oneof_def);
            ok(containing_type == msg_def, "PerlUpb_OneofDef_ContainingType");

            is(PerlUpb_OneofDef_Index(aTHX_ oneof_def), 0, "PerlUpb_OneofDef_Index");
            ok(!PerlUpb_OneofDef_IsSynthetic(aTHX_ oneof_def), "PerlUpb_OneofDef_IsSynthetic");

            const upb_FieldDef *f1 = PerlUpb_OneofDef_LookupName(aTHX_ oneof_def, "oneof_string");
            ok(f1 != NULL, "PerlUpb_OneofDef_LookupName('oneof_string')");

            const upb_FieldDef *f2 = PerlUpb_OneofDef_LookupNumber(aTHX_ oneof_def, 4); // oneof_int32 is field 4
            ok(f2 != NULL, "PerlUpb_OneofDef_LookupNumber(4)");

            const upb_FieldDef *f3 = PerlUpb_OneofDef_Field(aTHX_ oneof_def, 0);
            ok(f3 != NULL, "PerlUpb_OneofDef_Field(0)");
        } else {
            fprintf(stderr, "# Skipping OneofDef tests as oneof_def is NULL\n");
        }
    } else {
        fprintf(stderr, "# Skipping OneofDef tests as msg_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
