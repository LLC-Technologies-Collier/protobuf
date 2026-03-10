#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/protobuf/obj_cache.h"
#include "libcoro/coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For usleep

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

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX; // Declare my_perl
    char key[64];
    SV *val;

    for (int i = 0; i < NUM_OPS; i++) {
        sprintf(key, "coro%d_key%d", carg->id, i % 10);
        val = newSViv(carg->id * 10000 + i);

        protobuf_register_object(aTHX_ key, val);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

        SV *retrieved = protobuf_get_object(aTHX_ key);
        if (!retrieved || SvIV(retrieved) != SvIV(val)) {
            fprintf(stderr, "Coro %d: Error, key %s mismatch\n", carg->id, key);
            carg->errors++;
        }
        if (retrieved) SvREFCNT_dec(retrieved);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

        protobuf_unregister_object(aTHX_ key);
        SvREFCNT_dec(val);
        if (i % 50 == 0) {
            coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield more often
        }
    }
}

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(3 + NUM_COROS);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    protobuf_init_obj_cache(aTHX);
    ok(1, "protobuf_init_obj_cache called");

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

    // log something here? ("Running C-level coro stress test for obj_cache...");
    for (int j = 0; j < NUM_OPS * 2 * NUM_COROS; j++) {
        int coro_idx = j % NUM_COROS;
        coro_transfer(&main_ctx, coro_ctxs[coro_idx]);
    }
    // log something here? ("Finished C-level coro stress test.");

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

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}