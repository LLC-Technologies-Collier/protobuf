#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/enum.h"
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

    plan(2 + 4); // Adjusted plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fprintf(stderr, "Failed to load test descriptors");
         exit(1);
    }
    ok(1, "Descriptors loaded");

    const upb_EnumDef *enum_def = upb_DefPool_FindEnumByName(test_pool, "test.TestEnum");
    ok(enum_def, "Found test.TestEnum");

    if (enum_def) {
        const char* name = PerlUpb_EnumDef_Name(aTHX_ enum_def);
        is_string(name, "TestEnum", "PerlUpb_EnumDef_Name");

        const char* full_name = PerlUpb_EnumDef_FullName(aTHX_ enum_def);
        is_string(full_name, "test.TestEnum", "PerlUpb_EnumDef_FullName");

        int value_count = PerlUpb_EnumDef_ValueCount(aTHX_ enum_def);
        is(value_count, 3, "PerlUpb_EnumDef_ValueCount");

        const upb_EnumValueDef* val0 = PerlUpb_EnumDef_Value(aTHX_ enum_def, 0);
        ok(val0, "PerlUpb_EnumDef_Value(0)");
    } else {
        fprintf(stderr, "# Skipping EnumDef tests as enum_def is NULL");
        ok(1, "Skip"); ok(1, "Skip"); ok(1, "Skip"); ok(1, "Skip");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
