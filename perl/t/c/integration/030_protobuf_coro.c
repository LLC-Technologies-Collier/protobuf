#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/protobuf/obj_cache.h"
#include "libcoro/coro.h"
#include "t/c/coro_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NUM_COROS 50
#define NUM_OPS 1000
#define STACK_SIZE 16384

DECLARE_CORO_STATE()

typedef struct {
    int id;
    int errors;
} coro_arg_t;

// Some dummy objects to use as pointers
int dummy_objects[NUM_COROS][10];

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    SV *val;

    for (int i = 0; i < NUM_OPS; i++) {
        const void* ptr = &dummy_objects[carg->id - 1][i % 10];
        val = newSViv(carg->id * 10000 + i);
        SV* rv = newRV_noinc(val);

        PerlUpb_ObjCache_Add(aTHX_ ptr, rv);
        coro_yield(carg->id);

        SV *retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr);
        if (!retrieved_rv || SvIV(SvRV(retrieved_rv)) != (carg->id * 10000 + i)) {
            fprintf(stderr, "Coro %d: Error, pointer %p mismatch\n", carg->id, ptr);
            carg->errors++;
        }
        if (retrieved_rv) SvREFCNT_dec(retrieved_rv);
        coro_yield(carg->id);

        PerlUpb_ObjCache_Delete(aTHX_ ptr);
        SvREFCNT_dec(rv);
        if (i % 50 == 0) {
            coro_yield(carg->id);
        }
    }
    coro_finish(carg->id);
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(3 + NUM_COROS);

    PerlUpb_ObjCache_Init(aTHX);
    ok(1, "PerlUpb_ObjCache_Init called");

    coro_arg_t args[NUM_COROS];
    RUN_CORO_TEST(coro_test_func, args);

    test_perl_destroy(my_perl);
    return 0;
}
