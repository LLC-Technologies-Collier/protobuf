#include "EXTERN.h"
#include "perl.h"
#include "xs/protobuf.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include <string.h>

void xs_init(pTHX);

int main(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;

    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, xs_init, 3, embedding, NULL);
    perl_run(my_perl);

    plan(7);

    // Test PerlUpb_Arena_New
    SV* arena_sv = PerlUpb_Arena_New(aTHX);
    ok(arena_sv != NULL, "PerlUpb_Arena_New returns non-NULL");
    ok(SvROK(arena_sv) && SvIOK(SvRV(arena_sv)), "Arena SV is an IV ref");
    SvREFCNT_inc(arena_sv); // Keep it alive for the whole test

    // Test PerlUpb_Arena_Get
    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
    ok(arena != NULL, "PerlUpb_Arena_Get returns non-NULL upb_Arena");

    // Test allocation on the arena
    void *mem1 = upb_Arena_Malloc(arena, 128);
    ok(mem1 != NULL, "upb_Arena_Malloc allocates memory");
    strcpy((char*)mem1, "Hello Arena");
    is_string(mem1, "Hello Arena", "Memory on arena is usable");

    void *mem2 = upb_Arena_Malloc(arena, 64);
    ok(mem2 != NULL, "upb_Arena_Malloc allocates more memory");
    ok(mem1 != mem2, "Consecutive allocations are different");

    // Test Free and Destroy
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    // Check that the wrapper pointer is cleared
    ok(SvIV(SvRV(arena_sv)) == 0, "Wrapper pointer cleared after Destroy");

    SvREFCNT_dec(arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);

    return 0;
}

void xs_init(pTHX) { /* Effectively empty */ }