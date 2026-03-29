#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor/file.h"
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

    plan(2 + 18); // Initial plan

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_FileDef *file_def = upb_DefPool_FindFileByName(test_pool, "t/c/test.proto");
    ok(file_def != NULL, "Found t/c/test.proto");

    if (file_def) {
        is_string(PerlUpb_FileDef_Name(aTHX_ file_def), "t/c/test.proto", "PerlUpb_FileDef_Name");
        is_string(PerlUpb_FileDef_Package(aTHX_ file_def), "test", "PerlUpb_FileDef_Package");

        int dep_count = PerlUpb_FileDef_DependencyCount(aTHX_ file_def);
        ok(dep_count >= 0, "PerlUpb_FileDef_DependencyCount");

        if (dep_count > 0) {
            const upb_FileDef *dep = PerlUpb_FileDef_Dependency(aTHX_ file_def, 0);
            ok(dep != NULL, "PerlUpb_FileDef_Dependency");
        } else {
            ok(PerlUpb_FileDef_Dependency(aTHX_ file_def, 0) == NULL, "PerlUpb_FileDef_Dependency (null)");
        }

        ok(PerlUpb_FileDef_PublicDependencyCount(aTHX_ file_def) >= 0, "PerlUpb_FileDef_PublicDependencyCount");
        ok(PerlUpb_FileDef_PublicDependency(aTHX_ file_def, 0) == NULL, "PerlUpb_FileDef_PublicDependency (null expected)");

        ok(PerlUpb_FileDef_WeakDependencyCount(aTHX_ file_def) >= 0, "PerlUpb_FileDef_WeakDependencyCount");
        ok(PerlUpb_FileDef_WeakDependency(aTHX_ file_def, 0) == NULL, "PerlUpb_FileDef_WeakDependency (null expected)");

        int msg_count = PerlUpb_FileDef_TopLevelMessageCount(aTHX_ file_def);
        ok(msg_count > 0, "PerlUpb_FileDef_TopLevelMessageCount");
        const upb_MessageDef *msg = PerlUpb_FileDef_TopLevelMessage(aTHX_ file_def, 0);
        ok(msg != NULL, "PerlUpb_FileDef_TopLevelMessage");

        int enum_count = PerlUpb_FileDef_TopLevelEnumCount(aTHX_ file_def);
        ok(enum_count > 0, "PerlUpb_FileDef_TopLevelEnumCount");
        const upb_EnumDef *enm = PerlUpb_FileDef_TopLevelEnum(aTHX_ file_def, 0);
        ok(enm != NULL, "PerlUpb_FileDef_TopLevelEnum");

        int ext_count = PerlUpb_FileDef_TopLevelExtensionCount(aTHX_ file_def);
        ok(ext_count >= 0, "PerlUpb_FileDef_TopLevelExtensionCount");
        if (ext_count > 0) {
            ok(PerlUpb_FileDef_TopLevelExtension(aTHX_ file_def, 0) != NULL, "PerlUpb_FileDef_TopLevelExtension");
        } else {
            ok(PerlUpb_FileDef_TopLevelExtension(aTHX_ file_def, 0) == NULL, "PerlUpb_FileDef_TopLevelExtension (null)");
        }

        int svc_count = PerlUpb_FileDef_ServiceCount(aTHX_ file_def);
        ok(svc_count >= 0, "PerlUpb_FileDef_ServiceCount");
        if (svc_count > 0) {
            ok(PerlUpb_FileDef_Service(aTHX_ file_def, 0) != NULL, "PerlUpb_FileDef_Service");
        } else {
            ok(PerlUpb_FileDef_Service(aTHX_ file_def, 0) == NULL, "PerlUpb_FileDef_Service (null)");
        }

        const upb_DefPool *pool = PerlUpb_FileDef_Pool(aTHX_ file_def);
        ok(pool == test_pool, "PerlUpb_FileDef_Pool");

    } else {
        fprintf(stderr, "# Skipping FileDef tests as file_def is NULL\n");
    }

    upb_Arena_Free(arena);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
