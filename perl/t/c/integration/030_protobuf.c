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
    plan(7);

    PerlUpb_ObjCache_Init(aTHX);
    ok(1, "Cache initialized");

    SV* arena_sv = PerlUpb_Arena_New(aTHX);
    ok(arena_sv && sv_derived_from(arena_sv, "Protobuf::Arena"), "Arena SV created");
    upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ arena_sv);
    ok(arena != NULL, "upb_Arena obtained");

    // Use the arena pointer itself as a key for testing cache
    char* dummy = (char*)upb_Arena_Malloc(arena, 1);
    SV* val_sv = newSVpv("arena value", 0);
    SV* rv = newRV_noinc(val_sv);

    PerlUpb_ObjCache_Add(aTHX_ dummy, rv);

    SV* retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ dummy);
    ok(retrieved_rv != NULL, "Retrieved from cache");
    is_string(SvPV_nolen(SvRV(retrieved_rv)), "arena value", "Value from arena correct");
    SvREFCNT_dec(retrieved_rv);
    SvREFCNT_dec(rv);

    PerlUpb_Arena_Destroy(aTHX_ arena_sv); // This frees the arena and the wrapper
    ok(1, "Arena freed");

    TODO("Implement more comprehensive interaction tests") {
        ok(0, "Interactions between obj_cache, arena, and utils covered");
    }
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    { // Scope for test logic
        dSP;
        ENTER;
        SAVETMPS;

        test_arena_cache_interaction(aTHX);

        FREETMPS;
        LEAVE;
    } // End scope

    PerlUpb_ObjCache_Clear(aTHX);

    test_perl_destroy(my_perl);
    return 0;
}
