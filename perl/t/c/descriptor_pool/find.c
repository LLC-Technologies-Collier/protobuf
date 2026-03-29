#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor_pool/find.h"
#include "xs/descriptor/file.h"
#include "xs/descriptor/message.h"
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

    plan(5);

    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    // Wrap the test_pool
    SV* pool_sv = PerlUpb_DescriptorPool_GetWrapper(aTHX_ test_pool);
    ok(pool_sv != NULL, "Wrapped test_pool");

    // Test FindFileByName
    SV* file_sv = PerlUpb_DescriptorPool_FindFileByName(aTHX_ pool_sv, "test.proto");
    ok(sv_derived_from(file_sv, "Protobuf::FileDescriptor"), "Found file");
    SvREFCNT_dec(file_sv);

    // Test FindMessageByName
    SV* msg_sv = PerlUpb_DescriptorPool_FindMessageByName(aTHX_ pool_sv, "test.TestMessage");
    ok(sv_derived_from(msg_sv, "Protobuf::MessageDescriptor"), "Found message");
    SvREFCNT_dec(msg_sv);

    // Test Find non-existent
    SV* null_sv = PerlUpb_DescriptorPool_FindMessageByName(aTHX_ pool_sv, "non.Existent");
    ok(!SvOK(null_sv), "Find non-existent returns undef");
    SvREFCNT_dec(null_sv);

    // Cleanup
    SvREFCNT_dec(pool_sv);
    upb_Arena_Free(arena);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
