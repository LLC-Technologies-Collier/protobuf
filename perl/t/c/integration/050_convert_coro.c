#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/convert.h"
#include "xs/protobuf/arena.h" // For PerlUpb_Arena_New
#include "t/c/convert/test_util.h"
#include "libcoro/coro.h"
#include "t/c/coro_util.h"
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

DECLARE_CORO_STATE()

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

    coro_yield(carg->id);

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
        coro_yield(carg->id);
    }
    coro_finish(carg->id);
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(2 + NUM_COROS + 3);

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

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        args[i].arena = arena;
        args[i].arena_sv = arena_sv;
        args[i].malli = malli;
    }
    RUN_CORO_TEST(coro_test_func, args);

    TODO("Stress concurrent type-mismatch failures in SvToUpb") {
        ok(0, "System remains stable when multiple coroutines trigger conversion errors");
    }

    TODO("Verify integrated cache stability under high concurrent load") {
        ok(0, "Message wrapping and cache lookup are safe across interleaved coroutines");
    }

    TODO("Implement concurrent memory pressure stress during string conversion") {
        ok(0, "Arena growth and SV allocation remain robust under concurrency");
    }

    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    test_perl_destroy(my_perl);
    return 0;
}