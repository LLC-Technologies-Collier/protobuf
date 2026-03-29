#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/service.h"
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

    plan(2 + 8); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_ServiceDef *service_def = upb_DefPool_FindServiceByName(test_pool, "test.TestService");
    ok(service_def != NULL, "Found test.TestService");

    if (service_def) {
        is_string(PerlUpb_ServiceDef_FullName(aTHX_ service_def), "test.TestService", "PerlUpb_ServiceDef_FullName");
        is_string(PerlUpb_ServiceDef_Name(aTHX_ service_def), "TestService", "PerlUpb_ServiceDef_Name");
        is(PerlUpb_ServiceDef_Index(aTHX_ service_def), 0, "PerlUpb_ServiceDef_Index");
        is(PerlUpb_ServiceDef_MethodCount(aTHX_ service_def), 1, "PerlUpb_ServiceDef_MethodCount");
        
        const upb_MethodDef *m1 = PerlUpb_ServiceDef_Method(aTHX_ service_def, 0);
        ok(m1 != NULL, "PerlUpb_ServiceDef_Method(0)");
        
        const upb_MethodDef *m2 = PerlUpb_ServiceDef_FindMethodByName(aTHX_ service_def, "TestRpc");
        ok(m2 == m1, "PerlUpb_ServiceDef_FindMethodByName('TestRpc')");
        
        const upb_FileDef *file = PerlUpb_ServiceDef_File(aTHX_ service_def);
        ok(file != NULL, "PerlUpb_ServiceDef_File");
        
        is_string(upb_FileDef_Name(file), "test.proto", "File name matches");
    } else {
        fprintf(stderr, "# Skipping ServiceDef tests as service_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
