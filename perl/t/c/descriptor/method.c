#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/method.h"
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

    plan(2 + 9); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_ServiceDef *service_def = upb_DefPool_FindServiceByName(test_pool, "test.TestService");
    ok(service_def != NULL, "Found test.TestService");

    if (service_def) {
        const upb_MethodDef *method_def = upb_ServiceDef_Method(service_def, 0);
        ok(method_def != NULL, "Found Method 0");

        if (method_def) {
            is_string(PerlUpb_MethodDef_FullName(aTHX_ method_def), "test.TestService.TestRpc", "PerlUpb_MethodDef_FullName");
            is_string(PerlUpb_MethodDef_Name(aTHX_ method_def), "TestRpc", "PerlUpb_MethodDef_Name");
            is(PerlUpb_MethodDef_Index(aTHX_ method_def), 0, "PerlUpb_MethodDef_Index");
            
            const upb_MessageDef *input_type = PerlUpb_MethodDef_InputType(aTHX_ method_def);
            ok(input_type != NULL, "PerlUpb_MethodDef_InputType");
            is_string(upb_MessageDef_Name(input_type), "TestMessage", "InputType name");

            const upb_MessageDef *output_type = PerlUpb_MethodDef_OutputType(aTHX_ method_def);
            ok(output_type != NULL, "PerlUpb_MethodDef_OutputType");
            is_string(upb_MessageDef_Name(output_type), "AnotherMessage", "OutputType name");

            ok(!PerlUpb_MethodDef_ClientStreaming(aTHX_ method_def), "PerlUpb_MethodDef_ClientStreaming");
            ok(!PerlUpb_MethodDef_ServerStreaming(aTHX_ method_def), "PerlUpb_MethodDef_ServerStreaming");

            const upb_ServiceDef *svc = PerlUpb_MethodDef_Service(aTHX_ method_def);
            ok(svc == service_def, "PerlUpb_MethodDef_Service");
        } else {
            fprintf(stderr, "# Skipping MethodDef tests as method_def is NULL\n");
        }
    } else {
        fprintf(stderr, "# Skipping MethodDef tests as service_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
