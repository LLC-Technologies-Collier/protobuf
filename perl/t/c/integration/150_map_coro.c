#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/map/map.h"
#include "xs/map/iterator.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "libcoro/coro.h"
#include "t/c/convert/test_util.h"
#include "upb/reflection/message.h"
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
    SV* map_sv;
} coro_arg_t;

void test_map_access(pTHX_ coro_arg_t *carg) {
    SV* key_sv = newSViv(carg->id);
    SV* val_sv = newSViv(carg->id * 100);
    
    PerlUpb_Map_SetItem(aTHX_ carg->map_sv, key_sv, val_sv);
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    SV* ret_val = PerlUpb_Map_GetItem(aTHX_ carg->map_sv, key_sv);
    if (!SvIOK(ret_val) || SvIV(ret_val) != (carg->id * 100)) {
        fprintf(stderr, "Coro %d: Map value mismatch\n", carg->id);
        carg->errors++;
    }
    SvREFCNT_dec(ret_val);
    SvREFCNT_dec(key_sv);
    SvREFCNT_dec(val_sv);
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    for (int i = 0; i < NUM_OPS; i++) {
        test_map_access(aTHX_ carg);
        coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield
    }
    coro_active[carg->id - 1] = 0;
    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx);
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(2 + NUM_COROS);

    extern void PerlUpb_ObjCache_Init(pTHX);
    PerlUpb_ObjCache_Init(aTHX);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
    if (!load_test_descriptors(aTHX_ arena)) return 1;
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    const upb_FieldDef *map_field = upb_MessageDef_FindFieldByName(mdef, "map_int32_int32");
    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    upb_Map* map_ptr = upb_Message_Mutable(msg, map_field, arena).map;
    SV* map_sv = PerlUpb_Map_New(aTHX_ map_ptr, map_field, arena_sv);

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        coro_stack_alloc(&coro_stacks[i], STACK_SIZE);
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].map_sv = map_sv;
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

    extern void PerlUpb_Map_Free(pTHX_ SV* sv);
    PerlUpb_Map_Free(aTHX_ map_sv);
    SvREFCNT_dec(map_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    test_perl_destroy(my_perl);
    return 0;
}
