#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/message/serialize.h"
#include "xs/message/compare.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
#include "libcoro/coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NUM_COROS 10
#define NUM_OPS 20
#define STACK_SIZE 65536

coro_context main_ctx;
coro_context *coro_ctxs[NUM_COROS];
struct coro_stack coro_stacks[NUM_COROS];
int coro_active[NUM_COROS];

typedef struct {
    int id;
    int errors;
    SV* mdef_sv;
} coro_arg_t;

void test_message_ops(pTHX_ coro_arg_t *carg) {
    // 1. Create message
    SV* msg_sv = PerlUpb_Message_NewMessage(aTHX_ carg->mdef_sv);
    if (!msg_sv || !sv_isobject(msg_sv)) {
        carg->errors++;
        return;
    }

    const upb_MessageDef* mdef = PerlUpb_MessageDef_GetMessage(aTHX_ carg->mdef_sv);
    const upb_FieldDef* f_int32 = upb_MessageDef_FindFieldByName(mdef, "optional_int32");

    // 2. Set/Get
    char buf[32];
    sprintf(buf, "val %d", carg->id);
    SV* val_sv = newSViv(carg->id * 1000);
    PerlUpb_Message_SetField(aTHX_ msg_sv, f_int32, val_sv);
    
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    SV* ret_val = PerlUpb_Message_GetField(aTHX_ msg_sv, f_int32);
    if (SvIV(ret_val) != (carg->id * 1000)) {
        carg->errors++;
    }
    SvREFCNT_dec(ret_val);

    // 3. Serialize/Parse
    SV* serialized = PerlUpb_Message_Serialize(aTHX_ msg_sv);
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    SV* parsed_sv = PerlUpb_Message_Parse(aTHX_ carg->mdef_sv, serialized);
    if (!PerlUpb_Message_IsEqual(aTHX_ msg_sv, parsed_sv)) {
        carg->errors++;
    }

    // Cleanup
    SvREFCNT_dec(val_sv);
    SvREFCNT_dec(serialized);
    
    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ msg_sv));
    PerlUpb_Message_Free(aTHX_ msg_sv);
    SvREFCNT_dec(msg_sv);

    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ parsed_sv));
    PerlUpb_Message_Free(aTHX_ parsed_sv);
    SvREFCNT_dec(parsed_sv);
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    for (int i = 0; i < NUM_OPS; i++) {
        test_message_ops(aTHX_ carg);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield
    }
    coro_active[carg->id - 1] = 0;
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx);
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

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
    if (!load_test_descriptors(aTHX_ arena)) return 1;
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    SV* mdef_sv = PerlUpb_MessageDef_GetWrapper(aTHX_ mdef);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        coro_stack_alloc(&coro_stacks[i], STACK_SIZE);
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].mdef_sv = mdef_sv;
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

    SvREFCNT_dec(mdef_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
