#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor_pool/pool.h"

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(3);

    // 1. Creation
    SV* pool_sv = PerlUpb_DescriptorPool_New(aTHX);
    ok(pool_sv != NULL, "PerlUpb_DescriptorPool_New returns non-NULL");
    ok(sv_derived_from(pool_sv, "Protobuf::DescriptorPool"), "Pool SV has correct class");

    // 2. Retrieval of raw pool
    const upb_DefPool* raw_pool = PerlUpb_DescriptorPool_GetPool(aTHX_ pool_sv);
    ok(raw_pool != NULL, "PerlUpb_DescriptorPool_GetPool returns raw upb_DefPool");

    // Cleanup
    SvREFCNT_dec(pool_sv);

    test_perl_destroy(my_perl);
    return 0;
}
