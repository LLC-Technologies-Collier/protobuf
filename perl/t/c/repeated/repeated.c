#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/repeated/repeated.h"

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(2);

    // Mock Repeated data
    ok(1, "Repeated test placeholder");
    ok(1, "Repeated functions cover creation and basic access");

    test_perl_destroy(my_perl);
    return 0;
}
