#include "t/c/upb-perl-test.h"

int test_num = 0;
const char* todo_reason = NULL;

PerlInterpreter* test_perl_init(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);
    return my_perl;
}

void test_perl_destroy(PerlInterpreter *my_perl) {
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
}
