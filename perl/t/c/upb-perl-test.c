#include "t/c/upb-perl-test.h"
#include "xs/protobuf/registry.h"
#include "xs/protobuf/obj_cache.h"

int test_num = 0;
int indent_level = 0;
const char* todo_reason = NULL;

static int active_interpreters = 0;

PerlInterpreter* test_perl_init(int argc, char** argv) {
    static int sys_init_done = 0;
    if (!sys_init_done) {
        PERL_SYS_INIT(&argc, &argv);
        sys_init_done = 1;
    }
    active_interpreters++;
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    {
        dTHX;
        PerlUpb_Registry_Init(aTHX);
        PerlUpb_ObjCache_Init(aTHX);
    }

    return my_perl;
}

void test_perl_destroy(PerlInterpreter *my_perl) {
    perl_destruct(my_perl);
    perl_free(my_perl);
    active_interpreters--;
    if (active_interpreters == 0) {
        PERL_SYS_TERM();
    }
}
