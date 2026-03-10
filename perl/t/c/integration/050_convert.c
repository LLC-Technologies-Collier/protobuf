#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/convert/sv_to_upb.h"
#include "xs/convert/upb_to_sv.h"
#include "t/c/convert/test_util.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"
#include "upb/wire/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h> // For UINT32_MAX, etc.

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"



static void test_int32_roundtrip(pTHX_ upb_Arena *arena, SV *arena_sv) {
    const upb_FieldDef *f = get_field_def("protobuf_test_messages.proto2.TestAllTypesProto2", "optional_int32"); 
    ok(f, "Roundtrip/int32: Got FieldDef");
    if (!f) return;

    SV *orig_sv = newSViv(987);
    upb_MessageValue val;
    ok(PerlUpb_SvToUpb(aTHX_ orig_sv, f, &val, arena), "Roundtrip/int32: SvToUpb success");

    SV *new_sv = PerlUpb_UpbToSv(aTHX_ &val, f, arena_sv);
    ok(SvIOK(new_sv), "Roundtrip/int32: new_sv is IOK");
    is(SvIV(new_sv), 987, "Roundtrip/int32: value matches");
    SvREFCNT_dec(orig_sv);
    // new_sv is mortal
}

static void test_string_roundtrip(pTHX_ upb_Arena *arena, SV *arena_sv) {
    const upb_FieldDef *f = get_field_def("protobuf_test_messages.proto2.TestAllTypesProto2", "optional_string");
    ok(f, "Roundtrip/string: Got FieldDef");
    if (!f) return;

    const char *str = "Roundtrip Test";
    SV *orig_sv = newSVpvn(str, strlen(str));
    upb_MessageValue val;
    ok(PerlUpb_SvToUpb(aTHX_ orig_sv, f, &val, arena), "Roundtrip/string: SvToUpb success");
    // Check that the string was copied to the arena
    ok(val.str_val.data != str, "Roundtrip/string: String data was copied to arena");

    SV *new_sv = PerlUpb_UpbToSv(aTHX_ &val, f, arena_sv);
    ok(SvPOK(new_sv), "Roundtrip/string: new_sv is POK");
    is_string(SvPV_nolen(new_sv), str, "Roundtrip/string: value matches");
    SvREFCNT_dec(orig_sv);
    // new_sv is mortal
}

int main(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) {
        return 1;
    }

    plan(1 + 4 + 5);
    test_num = 0;
    ok(1, "Descriptors loaded");

    test_int32_roundtrip(aTHX_ arena, arena_sv);
    test_string_roundtrip(aTHX_ arena, arena_sv);

    upb_DefPool_Free(test_pool);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}