#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include <setjmp.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

// Dummy function to test croak
static void croak_me(pTHX_ const char *msg) {
    croak(msg);
}

static void test_jmpenv(pTHX) {
    dJMPENV;
    int ret;

    plan(5);

    // Test successful croak catch
    JMPENV_PUSH(ret);
    if (ret == 0) {
        croak_me(aTHX_ "Intentional croak");
        JMPENV_POP;
        fail("Croak test: croak_me() did not jump");
    } else {
        JMPENV_POP;
        ok(1, "Croak test: Caught croak");
        sv_setsv(ERRSV, &PL_sv_undef); // Clear error
    }

    // Test block without croak
    JMPENV_PUSH(ret); // Push a new context
    if (ret == 0) {
        ok(1, "No croak test: Inside JMPENV block");
        JMPENV_POP;
        ok(1, "No croak test: JMPENV_POP called");
    } else {
        JMPENV_POP;
        fail("No croak test: Unexpected jump");
        sv_setsv(ERRSV, &PL_sv_undef);
    }
    ok(1, "Croak tests completed");
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    test_jmpenv(aTHX);

    test_perl_destroy(my_perl);
    return 0;
}
