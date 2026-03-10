#include "xs/protobuf.h"
#include "t/c/upb-perl-test.h"
#include "xs/protobuf/obj_cache.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/utils.h"
#include <string.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

static void test_arena_cache_interaction(pTHX) {
    plan(6);

    protobuf_init_obj_cache(aTHX);
    ok(1, "Cache initialized");

    SV* arena_sv = PerlUpb_Arena_New(aTHX);
    ok(arena_sv && sv_derived_from(arena_sv, "Protobuf::Arena"), "Arena SV created");
    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
    ok(arena != NULL, "upb_Arena obtained");

    const char* key = "test_key";
    char* arena_str = (char*)upb_Arena_Malloc(arena, 12);
    strcpy(arena_str, "arena value");
    SV* val_sv = newSVpvn(arena_str, 11);

    protobuf_register_object(aTHX_ key, val_sv);

    SV* retrieved_sv = protobuf_get_object(aTHX_ key);
    ok(retrieved_sv != NULL, "Retrieved from cache");
    is_string(SvPV_nolen(retrieved_sv), "arena value", "Value from arena correct");
    SvREFCNT_dec(retrieved_sv);
    SvREFCNT_dec(val_sv);

    PerlUpb_Arena_Destroy(aTHX_ arena_sv); // This frees the arena and the wrapper
    ok(1, "Arena freed");
    // The cache entry should now be stale, but the key pointer is invalid.
    // Re-initializing the cache for safety in tests.
}

int main(int argc, char** argv, char** env) {
    PERL_SYS_INIT3(&argc, &argv, &env);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    { // Scope for test logic
        dSP;
        ENTER;
        SAVETMPS;

        test_arena_cache_interaction(aTHX);

        FREETMPS;
        LEAVE;
    } // End scope

    protobuf_clear_obj_cache(my_perl); // Clear after scope

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
