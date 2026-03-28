#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/enum.h"       // To get EnumDef for testing
#include "xs/descriptor/enum_value.h"
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

    plan(2 + 1 + 3); // Adjusted plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fprintf(stderr, "Failed to load test descriptors");
         exit(1);
    }
    ok(1, "Descriptors loaded");

    const upb_EnumDef *enum_def = upb_DefPool_FindEnumByName(test_pool, "test.TestEnum");
    ok(enum_def, "Found test.TestEnum");

    if (enum_def) {
        const upb_EnumValueDef* val1 = upb_EnumDef_FindValueByName(enum_def, "TEST_ENUM_VAL1");
        ok(val1, "Found TEST_ENUM_VAL1");
        if (val1) {
            is_string(PerlUpb_EnumValueDef_Name(aTHX_ val1), "TEST_ENUM_VAL1", "PerlUpb_EnumValueDef_Name");
            is(PerlUpb_EnumValueDef_Number(aTHX_ val1), 1, "PerlUpb_EnumValueDef_Number");
            is(PerlUpb_EnumValueDef_Index(aTHX_ val1), 1, "PerlUpb_EnumValueDef_Index"); // Index should be 1 as UNKNOWN is 0
        } else {
            fprintf(stderr, "# Skipping EnumValueDef tests as val1 is NULL");
            ok(1, "Skip"); ok(1, "Skip"); ok(1, "Skip");
        }
    } else {
        fprintf(stderr, "# Skipping EnumValueDef tests as enum_def is NULL");
        ok(1, "Skip"); ok(1, "Skip"); ok(1, "Skip"); ok(1, "Skip");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
