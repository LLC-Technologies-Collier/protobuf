#include "EXTERN.h"
#include "perl.h"
#include "xs/protobuf.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include <string.h>

void xs_init(pTHX);

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(15);

    // Test PerlUpb_Arena_New
    SV* arena_sv = PerlUpb_Arena_New(aTHX);
    ok(arena_sv != NULL, "PerlUpb_Arena_New returns non-NULL");
    ok(SvROK(arena_sv) && SvTYPE(SvRV(arena_sv)) == SVt_PVHV, "Arena SV is a HASH ref");
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
    // Check that the wrapper pointer is cleared in the hash
    SV** svp = hv_fetch((HV*)SvRV(arena_sv), "_arena_ptr", 10, 0);
    ok(svp && SvIOK(*svp) && SvIV(*svp) == 0, "Wrapper pointer cleared after Destroy");

    TODO("Implement PerlUpb_Arena_Free tests") {
        ok(0, "PerlUpb_Arena_Free works as expected");
    }

    TODO("Implement raw arena function tests") {
        ok(0, "PerlUpb_Arena_CreateRaw / DestroyRaw / GetRaw work correctly");
    }

    TODO("Implement re-entrancy tests for arena") {
        ok(0, "arena operations are safe under re-entrancy");
    }

    TODO("Implement arena memory usage statistics (Allocated vs. Reserved)") {
        ok(0, "Observation API for arena size and overhead");
    }

    TODO("Implement tmpfs-backed custom allocators for zero-copy high-performance IPC") {
        ok(0, "Arena can be backed by shared memory segments");
    }

    TODO("Implement thread-local arena caching for ultra-high-frequency allocations") {
        ok(0, "Small allocations bypass global locks or complex state checks");
    }

    TODO("Add memory corruption guards (canary bytes) around arena blocks") {
        ok(0, "Out-of-bounds writes are detected by the core library");
    }

    SvREFCNT_dec(arena_sv);

    test_perl_destroy(my_perl);

    return 0;
}

void xs_init(pTHX) { /* Effectively empty */ }