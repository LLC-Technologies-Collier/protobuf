#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor_pool/find.h"
#include "xs/descriptor/message.h"
#include "libcoro/coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NUM_COROS 50
#define NUM_OPS 100
#define STACK_SIZE 65536

coro_context main_ctx;
coro_context *coro_ctxs[NUM_COROS];
struct coro_stack coro_stacks[NUM_COROS];
int coro_active[NUM_COROS];

typedef struct {
    int id;
    int errors;
    SV* pool_sv;
} coro_arg_t;

void test_pool_access(pTHX_ coro_arg_t *carg) {
    SV* msg_sv = PerlUpb_DescriptorPool_FindMessageByName(aTHX_ carg->pool_sv, "Test");
    if (!msg_sv || !SvOK(msg_sv)) {
        carg->errors++;
        return;
    }
    
    if (!sv_derived_from(msg_sv, "Protobuf::Descriptor::MessageDef")) {
        carg->errors++;
    }
    
    SV* msg_sv_again = PerlUpb_DescriptorPool_FindMessageByName(aTHX_ carg->pool_sv, "Test");
    if (SvRV(msg_sv_again) != SvRV(msg_sv)) {
        carg->errors++;
    }

    SvREFCNT_dec(msg_sv);
    SvREFCNT_dec(msg_sv_again);
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    for (int i = 0; i < NUM_OPS; i++) {
        test_pool_access(aTHX_ carg);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield
    }
    coro_active[carg->id - 1] = 0;
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Final return
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

    extern void PerlUpb_ObjCache_Init(pTHX);
    PerlUpb_ObjCache_Init(aTHX);

    SV* pool_sv = PerlUpb_DescriptorPool_GeneratedPool(aTHX);
    ok(pool_sv != NULL, "Got GeneratedPool");

    unsigned char test_proto[] = {
        0x0a, 0x0a, 't', 'e', 's', 't', '.', 'p', 'r', 'o', 't', 'o',
        0x22, 0x06, 0x0a, 0x04, 'T', 'e', 's', 't'
    };
    SV* serialized = newSVpvn((const char*)test_proto, sizeof(test_proto));
    extern SV* PerlUpb_DescriptorPool_AddSerializedFile(pTHX_ SV* self, SV* serialized);
    SV* file_sv = PerlUpb_DescriptorPool_AddSerializedFile(aTHX_ pool_sv, serialized);
    ok(file_sv != NULL, "Added test message to pool");
    SvREFCNT_dec(file_sv);
    SvREFCNT_dec(serialized);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        coro_stack_alloc(&coro_stacks[i], STACK_SIZE);
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].pool_sv = pool_sv;
        coro_active[i] = 1;
        coro_ctxs[i] = (coro_context *)malloc(sizeof(coro_context));
        coro_create(coro_ctxs[i], coro_test_func, &args[i], coro_stacks[i].sptr, coro_stacks[i].ssze);
    }

    int active_count = NUM_COROS;
    int j = 0;
    while (active_count > 0) {
        int idx = j % NUM_COROS;
        if (coro_active[idx]) {
            coro_transfer(&main_ctx, coro_ctxs[idx]);
            if (!coro_active[idx]) active_count--;
        }
        j++;
    }

    int total_errors = 0;
    for (int i = 0; i < NUM_COROS; i++) {
        total_errors += args[i].errors;
        ok(args[i].errors == 0, "Coro completed without errors");
        coro_stack_free(&coro_stacks[i]);
        free(coro_ctxs[i]);
    }
    is(total_errors, 0, "Total errors from all coroutines");

    SvREFCNT_dec(pool_sv);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
