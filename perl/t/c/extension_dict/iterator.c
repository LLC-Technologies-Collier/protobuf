#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/extension_dict/iterator.h"

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(1);

    TODO("Implement ExtensionDict Iterator unit tests") {
        ok(0, "Extension iterator logic tested");
    }

    test_perl_destroy(my_perl);
    return 0;
}
