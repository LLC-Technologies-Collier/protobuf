#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/repeated/composite.h"

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(1);

    TODO("Implement composite repeated field unit tests") {
        ok(0, "Composite repeated field logic tested");
    }

    test_perl_destroy(my_perl);
    return 0;
}
