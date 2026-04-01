#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/protobuf/obj_cache.h"
#include "libcoro/coro.h"
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

coro_context main_ctx;
coro_context *coro_ctxs[NUM_COROS];
struct coro_stack coro_stacks[NUM_COROS];

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
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

        SV *retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr);
        if (!retrieved_rv || SvIV(SvRV(retrieved_rv)) != (carg->id * 10000 + i)) {
            fprintf(stderr, "Coro %d: Error, pointer %p mismatch\n", carg->id, ptr);
            carg->errors++;
        }
        if (retrieved_rv) SvREFCNT_dec(retrieved_rv);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

        PerlUpb_ObjCache_Delete(aTHX_ ptr);
        SvREFCNT_dec(rv);
        if (i % 50 == 0) {
            coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield more often
        }
    }
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(3 + NUM_COROS);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    PerlUpb_ObjCache_Init(aTHX);
    ok(1, "PerlUpb_ObjCache_Init called");

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        if (!coro_stack_alloc(&coro_stacks[i], STACK_SIZE)) {
            perror("coro_stack_alloc");
            return 1;
        }
        args[i].id = i + 1;
        args[i].errors = 0;
        coro_ctxs[i] = (coro_context *)malloc(sizeof(coro_context));
        coro_create(coro_ctxs[i], coro_test_func, &args[i], coro_stacks[i].sptr, coro_stacks[i].ssze);
    }

    for (int j = 0; j < NUM_OPS * 2 * NUM_COROS; j++) {
        int coro_idx = j % NUM_COROS;
        coro_transfer(&main_ctx, coro_ctxs[coro_idx]);
    }

    int total_errors = 0;
    for (int i = 0; i < NUM_COROS; i++) {
        total_errors += args[i].errors;
        char test_name[100];
        sprintf(test_name, "Coro %d completed without errors", i + 1);
        ok(args[i].errors == 0, test_name);
        coro_stack_free(&coro_stacks[i]);
        free(coro_ctxs[i]);
    }

    is(total_errors, 0, "Total errors from all coroutines");

    test_perl_destroy(my_perl);
    return 0;
}
