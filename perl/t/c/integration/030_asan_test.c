#include "t/c/upb-perl-test.h"
#include <setjmp.h>
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

int main(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    {
        dSP; ENTER; SAVETMPS;
        dJMPENV;
        int ret;
        JMPENV_PUSH(ret);
        if (ret == 0) {
            croak("Test croak");
            JMPENV_POP;
        } else {
            JMPENV_POP;
            sv_setsv_mg(ERRSV, &PL_sv_undef);
        }
        FREETMPS; LEAVE;
    }

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
