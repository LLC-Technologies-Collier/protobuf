#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/extension_dict/dict.h"
#include "xs/extension_dict/iterator.h"
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

#define NUM_COROS 20
#define NUM_OPS 50
#define STACK_SIZE 65536

coro_context main_ctx;
coro_context *coro_ctxs[NUM_COROS];
struct coro_stack coro_stacks[NUM_COROS];
int coro_active[NUM_COROS];

typedef struct {
    int id;
    int errors;
    SV* dict_sv;
    SV* field_sv;
} coro_arg_t;

void test_extension_access(pTHX_ coro_arg_t *carg) {
    char val_str[32];
    sprintf(val_str, "coro %d op", carg->id);
    SV* val_sv = newSVpv(val_str, 0);
    
    // Set value
    PerlUpb_ExtensionDict_SetItem(aTHX_ carg->dict_sv, carg->field_sv, val_sv);
    
    // Yield immediately after set to let others overwrite
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); 

    // Get value. Note: in this specific test, multiple coros are fighting 
    // for the SAME field, so we might not get our own value back 
    // if another coro ran in between.
    // However, since we yield to main and then main transfers to the NEXT coro,
    // we CAN predict the behavior.
    
    SV* ret_val = PerlUpb_ExtensionDict_GetItem(aTHX_ carg->dict_sv, carg->field_sv);
    // (We don't assert the exact value here because of the contention, 
    // just that it's a valid string)
    if (!SvPOK(ret_val)) {
        carg->errors++;
    }
    
    SvREFCNT_dec(ret_val);
    SvREFCNT_dec(val_sv);
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    for (int i = 0; i < NUM_OPS; i++) {
        test_extension_access(aTHX_ carg);
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

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    const upb_FieldDef *ext_field = upb_DefPool_FindExtensionByName(test_pool, "test.extension_string");
    
    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    SV* message_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
    SV* dict_sv = PerlUpb_ExtensionDict_New(aTHX_ message_sv);
    SV* field_sv = PerlUpb_FieldDef_GetWrapper(aTHX_ ext_field);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        coro_stack_alloc(&coro_stacks[i], STACK_SIZE);
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].dict_sv = dict_sv;
        args[i].field_sv = field_sv;
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

    SvREFCNT_dec(field_sv);
    extern void PerlUpb_ExtensionDict_Free(pTHX_ SV* sv);
    PerlUpb_ExtensionDict_Free(aTHX_ dict_sv);
    SvREFCNT_dec(dict_sv);
    SvREFCNT_dec(message_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
