#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/descriptor_pool/pool.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(7);

    // Test New
    SV* pool_sv1 = PerlUpb_DescriptorPool_New(aTHX);
    ok(pool_sv1 != NULL, "Created new DescriptorPool");
    ok(sv_derived_from(pool_sv1, "Protobuf::DescriptorPool"), "Blessed correctly");
    
    const upb_DefPool* raw_pool1 = PerlUpb_DescriptorPool_GetPool(aTHX_ pool_sv1);
    ok(raw_pool1 != NULL, "Got raw pool");

    // Test Object Cache (GetWrapper for same raw pool)
    SV* pool_sv1_again = PerlUpb_DescriptorPool_GetWrapper(aTHX_ raw_pool1);
    is(SvRV(pool_sv1_again), SvRV(pool_sv1), "Object cache returns same SV for same raw pool");
    SvREFCNT_dec(pool_sv1_again);

    // Test GeneratedPool
    SV* gen_pool_sv = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
    ok(gen_pool_sv != NULL, "Got GeneratedPool");
    ok(SvRV(gen_pool_sv) != SvRV(pool_sv1), "GeneratedPool is different from new pool");
    
    SV* gen_pool_sv_again = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
    is(SvRV(gen_pool_sv_again), SvRV(gen_pool_sv), "GeneratedPool returns same singleton");
    SvREFCNT_dec(gen_pool_sv_again);

    // Cleanup
    extern void PerlUpb_DescriptorPool_Free(pTHX_ SV* sv);
    PerlUpb_DescriptorPool_Free(aTHX_ pool_sv1);
    SvREFCNT_dec(pool_sv1);
    
    PerlUpb_DescriptorPool_Free(aTHX_ gen_pool_sv); // This won't free the raw pool as it's the generated one.
    SvREFCNT_dec(gen_pool_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
