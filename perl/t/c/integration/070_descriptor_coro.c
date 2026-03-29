#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/message.h"
#include "xs/descriptor/field.h"
#include "xs/protobuf/arena.h"
#include "t/c/convert/test_util.h"
#include "libcoro/coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NUM_COROS 50
#define NUM_OPS 100
#define STACK_SIZE 32768

coro_context main_ctx;
coro_context *coro_ctxs[NUM_COROS];
struct coro_stack coro_stacks[NUM_COROS];

typedef struct {
    int id;
    int errors;
} coro_arg_t;

void test_descriptor_access(pTHX_ coro_arg_t *carg) {
    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    if (!msg_def) {
        fprintf(stderr, "Coro %d: Failed to find test.TestMessage\n", carg->id);
        carg->errors++;
        return;
    }

    const char* full_name = PerlUpb_MessageDef_FullName(aTHX_ msg_def);
    if (strcmp(full_name, "test.TestMessage") != 0) {
        fprintf(stderr, "Coro %d: MessageFullName mismatch\n", carg->id);
        carg->errors++;
    }

    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    const upb_FieldDef *field = PerlUpb_MessageDef_FindFieldByName(aTHX_ msg_def, "value");
    if (!field) {
        fprintf(stderr, "Coro %d: Failed to find value\n", carg->id);
        carg->errors++;
        return;
    }

    if (PerlUpb_FieldDef_Type(aTHX_ field) != kUpb_FieldType_Int32) {
        fprintf(stderr, "Coro %d: FieldType mismatch\n", carg->id);
        carg->errors++;
    }
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX; // Declare my_perl

    for (int i = 0; i < NUM_OPS; i++) {
        test_descriptor_access(aTHX_ carg);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield
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

    plan(2 + NUM_COROS);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) {
         fprintf(stderr, "Failed to load test descriptors\n");
         return 1;
    }
    ok(1, "Descriptors loaded");

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

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

    cdiag("Running C-level coro stress test for descriptor access...");
    for (int j = 0; j < NUM_OPS * 2 * NUM_COROS; j++) {
        int coro_idx = j % NUM_COROS;
        coro_transfer(&main_ctx, coro_ctxs[coro_idx]);
    }
    cdiag("Finished C-level coro stress test.");

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

    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
