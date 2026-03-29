#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor_containers/by_name_map.h"
#include "xs/descriptor_containers/generic_sequence.h"
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
    const upb_MessageDef *msg_def;
    SV* parent_sv;
} coro_arg_t;

// Re-use vtables from 090_descriptor_containers.c (could be shared in a real project)
int msg_field_count(const void* p) { return upb_MessageDef_FieldCount((const upb_MessageDef*)p); }
const void* msg_field_get(const void* p, int i) { return upb_MessageDef_Field((const upb_MessageDef*)p, i); }
SV* msg_field_wrap(pTHX_ const void* p) { return newSViv((IV)p); }
static const PerlUpb_GenericSequence_VTable msg_fields_seq_vtable = { msg_field_count, msg_field_get, msg_field_wrap };

const void* msg_field_lookup(const void* p, const char* n) { return upb_MessageDef_FindFieldByName((const upb_MessageDef*)p, n); }
const char* msg_field_key(const void* p, int i) {
    const upb_FieldDef* f = upb_MessageDef_Field((const upb_MessageDef*)p, i);
    return f ? upb_FieldDef_Name(f) : NULL;
}
const void* msg_field_value(const void* p, int i) { return upb_MessageDef_Field((const upb_MessageDef*)p, i); }
static const PerlUpb_ByNameMap_VTable msg_fields_map_vtable = { msg_field_count, msg_field_lookup, msg_field_key, msg_field_value, msg_field_wrap };

void test_container_access(pTHX_ coro_arg_t *carg) {
    // Test map creation and lookup
    SV* map_sv = PerlUpb_ByNameMap_New(aTHX_ carg->parent_sv, carg->msg_def, &msg_fields_map_vtable);
    if (!map_sv) { carg->errors++; return; }

    SV* val_sv = PerlUpb_ByNameMap_Lookup(aTHX_ map_sv, "value");
    if (!SvOK(val_sv)) {
        fprintf(stderr, "Coro %d: Failed to find 'value'\n", carg->id);
        carg->errors++;
    }
    SvREFCNT_dec(val_sv);

    coro_transfer(coro_ctxs[carg->id - 1], &main_ctx); // Yield

    // Test sequence creation and count
    SV* seq_sv = PerlUpb_GenericSequence_New(aTHX_ carg->parent_sv, carg->msg_def, &msg_fields_seq_vtable);
    if (!seq_sv) { carg->errors++; return; }

    if (PerlUpb_GenericSequence_Count(aTHX_ seq_sv) <= 0) {
        fprintf(stderr, "Coro %d: Field count <= 0\n", carg->id);
        carg->errors++;
    }
    
    // Explicitly free the map and sequence internal state for the test (normally DESTROY handles this)
    extern void PerlUpb_ByNameMap_Free(pTHX_ SV* sv);
    PerlUpb_ByNameMap_Free(aTHX_ map_sv);
    extern void PerlUpb_GenericSequence_Free(pTHX_ SV* sv);
    PerlUpb_GenericSequence_Free(aTHX_ seq_sv);

    SvREFCNT_dec(map_sv);
    SvREFCNT_dec(seq_sv);
}

void coro_test_func(void *arg) {
    coro_arg_t *carg = (coro_arg_t *)arg;
    dTHX;
    for (int i = 0; i < NUM_OPS; i++) {
        test_container_access(aTHX_ carg);
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
    if (!load_test_descriptors(aTHX_ arena)) { return 1; }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *msg_def = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    if (!msg_def) { return 1; }

    coro_create(&main_ctx, NULL, NULL, NULL, 0);

    SV* parent_sv = newSViv(1);
    coro_arg_t args[NUM_COROS];
    for (int i = 0; i < NUM_COROS; i++) {
        coro_stack_alloc(&coro_stacks[i], STACK_SIZE);
        args[i].id = i + 1;
        args[i].errors = 0;
        args[i].msg_def = msg_def;
        args[i].parent_sv = parent_sv;
        coro_ctxs[i] = (coro_context *)malloc(sizeof(coro_context));
        coro_create(coro_ctxs[i], coro_test_func, &args[i], coro_stacks[i].sptr, coro_stacks[i].ssze);
    }

    cdiag("Running C-level coro stress test for containers...");
    for (int j = 0; j < NUM_OPS * 2 * NUM_COROS; j++) {
        coro_transfer(&main_ctx, coro_ctxs[j % NUM_COROS]);
    }
    cdiag("Finished C-level coro stress test.");

    int total_errors = 0;
    for (int i = 0; i < NUM_COROS; i++) {
        total_errors += args[i].errors;
        ok(args[i].errors == 0, "Coro completed without errors");
        coro_stack_free(&coro_stacks[i]);
        free(coro_ctxs[i]);
    }
    is(total_errors, 0, "Total errors from all coroutines");

    SvREFCNT_dec(parent_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
