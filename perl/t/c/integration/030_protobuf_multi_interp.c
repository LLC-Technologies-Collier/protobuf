#include "perl/t/c/upb-perl-test.h"
#include "perl/xs/protobuf/registry.h"
#include "perl/xs/protobuf/obj_cache.h"

static void test_multi_interp(void) {
    plan(4);

    char *my_argv[] = { "", "-e", "0", NULL };

    PerlInterpreter *test_perl1 = perl_alloc();
    perl_construct(test_perl1);
    perl_parse(test_perl1, NULL, 3, my_argv, NULL);
    perl_run(test_perl1);
    PERL_SET_CONTEXT(test_perl1);
    dTHX;
    PerlUpb_Registry_Init(aTHX);
    PerlUpb_ObjCache_Init(aTHX);
    PerlUpb_Registry* reg1 = PerlUpb_Registry_Get(aTHX);
    void* dummy_ptr = (void*)0x1234;
    SV* obj1 = newSViv(42);
    PerlUpb_ObjCache_Add(aTHX, dummy_ptr, obj1);
    SvREFCNT_dec(obj1);

    PerlInterpreter *test_perl2 = perl_alloc();
    perl_construct(test_perl2);
    perl_parse(test_perl2, NULL, 3, my_argv, NULL);
    perl_run(test_perl2);
    PERL_SET_CONTEXT(test_perl2);
    {
        dTHX;
        PerlUpb_Registry_Init(aTHX);
        PerlUpb_ObjCache_Init(aTHX);
        PerlUpb_Registry* reg2 = PerlUpb_Registry_Get(aTHX);
        
        ok(reg1 != reg2, "Registries are isolated across interpreters");
        
        SV* cached = PerlUpb_ObjCache_Get(aTHX, dummy_ptr);
        ok(cached == NULL, "Cache is isolated: perl2 cannot see perl1 entries");
    }

    PERL_SET_CONTEXT(test_perl1);
    {
        dTHX;
        SV* cached = PerlUpb_ObjCache_Get(aTHX, dummy_ptr);
        ok(cached != NULL, "Cache persist in perl1");
    }

    perl_destruct(test_perl2);
    perl_free(test_perl2);

    PERL_SET_CONTEXT(test_perl1);
    {
        dTHX;
        SV* cached = PerlUpb_ObjCache_Get(aTHX, dummy_ptr);
        ok(cached != NULL, "Cache still persist in perl1 after perl2 death");
    }

    perl_destruct(test_perl1);
    perl_free(test_perl1);
}

int main(int argc, char **argv, char **env) {
    test_multi_interp();
    return 0;
}
