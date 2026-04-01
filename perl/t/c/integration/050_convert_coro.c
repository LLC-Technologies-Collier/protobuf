#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/convert.h"
#include "xs/protobuf/arena.h" // For PerlUpb_Arena_New
#include "t/c/convert/test_util.h"
#include "libcoro/coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For usleep

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
    upb_Arena *arena;
    SV *arena_sv;
    const upb_MessageDef *malli;
} coro_arg_t;

// Simplified test function for one type
void test_string_conversion(pTHX_ coro_arg_t *carg) {
    const upb_FieldDef *field = get_field_def("protobuf_test_messages.proto2.TestAllTypesProto2", "optional_string");
    if (!field) {
        fprintf(stderr, "Coro %d: Failed to get FieldDef for optional_string\n", carg->id);
        carg->errors++;
        return;
    }

    // SV to UPB
    SV *sv = newSVpvn("hello coro", 10);
    upb_MessageValue val;
    bool result = PerlUpb_SvToUpb(aTHX_ sv, field, &val, carg->arena);
    if (!result) {
        fprintf(stderr, "Coro %d: PerlUpb_SvToUpb failed for string\n", carg->id);
        carg->errors++;
    } else {
        if (memcmp(val.str_val.data, "hello coro", 10) != 0 || val.str_val.size != 10) {
            fprintf(stderr, "Coro %d: SV to UPB string conversion mismatch\n", carg->id);
            carg->errors++;
        }
    }
    SvREFCNT_dec(sv);

    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    // UPB to SV
    val.str_val = upb_StringView_FromString("upb to sv");
    SV *ret_sv = PerlUpb_UpbToSv(aTHX_ &val, field, carg->arena_sv);
    if (!ret_sv) {
        fprintf(stderr, "Coro %d: PerlUpb_UpbToSv failed for string\n", carg->id);
        carg->errors++;
    } else {
        if (!SvPOK(ret_sv) || strcmp(SvPV_nolen(ret_sv), "upb to sv") != 0) {
            fprintf(stderr, "Coro %d: UPB to SV string conversion mismatch\n", carg->id);
            carg->errors++;
        }
        SvREFCNT_dec(ret_sv);
    }
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX; // Declare my_perl

    for (int i = 0; i < NUM_OPS; i++) {
        test_string_conversion(aTHX_ carg);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield
    }
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(3 + NUM_COROS);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) {
         fprintf(stderr, "Failed to load test descriptors\n");
         return 1;
    }
     ok(1, "Descriptors loaded");

    const upb_MessageDef *malli = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    if (!malli) {
        fprintf(stderr, "Failed to find TestAllTypesProto2 message\n");
        return 1;
    }

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        if (!coro_stack_alloc(&coro_stacks[i], STACK_SIZE)) {
            perror("coro_stack_alloc");
            return 1;
        }
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].arena = arena;
        args[i].arena_sv = arena_sv;
        args[i].malli = malli;
        coro_ctxs[i] = (coro_context *)malloc(sizeof(coro_context));
        coro_create(coro_ctxs[i], coro_test_func, &args[i], coro_stacks[i].sptr, coro_stacks[i].ssze);
    }

    cdiag("Running C-level coro stress test for convert functions...");
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
    test_perl_destroy(my_perl);
    return 0;
}