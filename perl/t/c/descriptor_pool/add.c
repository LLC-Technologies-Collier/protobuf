#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor_pool/add.h"
#include "xs/descriptor/file.h"
#include <stdio.h>

// Minimal serialized FileDescriptorProto: name = "test.proto"
// Field 1 (name) = 0x0a, length 10 = 0x0a
unsigned char test_proto_data[] = {
    0x0a, 0x0a, 't', 'e', 's', 't', '.', 'p', 'r', 'o', 't', 'o'
};

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(3);

    SV* pool_sv = PerlUpb_DescriptorPool_New(aTHX);
    ok(pool_sv != NULL, "Created new pool");

    SV* serialized = newSVpvn((const char*)test_proto_data, sizeof(test_proto_data));
    
    SV* file_sv = PerlUpb_DescriptorPool_AddSerializedFile(aTHX_ pool_sv, serialized);
    ok(sv_derived_from(file_sv, "Protobuf::FileDescriptor"), "Added serialized file");
    
    const upb_FileDef* file_raw = PerlUpb_FileDef_GetFile(aTHX_ file_sv);
    is_string(upb_FileDef_Name(file_raw), "test.proto", "File name is correct");

    // Cleanup
    SvREFCNT_dec(file_sv);
    SvREFCNT_dec(serialized);
    
    extern void PerlUpb_DescriptorPool_Free(pTHX_ SV* sv);
    PerlUpb_DescriptorPool_Free(aTHX_ pool_sv);
    SvREFCNT_dec(pool_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
