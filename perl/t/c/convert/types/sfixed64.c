#include "t/c/convert/types/sfixed64.h"
#include "t/c/upb-perl-test.h"
#include "t/c/convert/test_util.h" // Add this
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>

// test_num is external from the main runner
extern int test_num;

// upb_to_sv
void set_sfixed64_val(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->int64_val = -123456789012345LL; }
void check_sv_sfixed64_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), -123456789012345LL, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_sfixed64_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Int64);
    upb_MessageValue msg_val;
    msg_val.int64_val = -9999LL;
    upb_Array_Append(arr, msg_val, arena);
    msg_val.int64_val = 8888LL;
    upb_Array_Append(arr, msg_val, arena);
    val->array_val = arr;
}

static void check_sv_repeated_sfixed64_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvROK(sv), sdiagnostic("%s: SV is a reference", prefix));
    if (!SvROK(sv)) return;
    SV *deref = SvRV(sv);
    ok(SvTYPE(deref) == SVt_PVAV, sdiagnostic("%s: Dereferenced SV is an ARRAY", prefix));
    if (SvTYPE(deref) != SVt_PVAV) return;

    AV *av = (AV*)deref;
    is(av_len(av), 1, sdiagnostic("%s: Array has 2 elements", prefix));
    SV **elem1 = av_fetch(av, 0, 0);
    ok(elem1 && *elem1, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem1 && *elem1) {
        ok(SvIOK(*elem1), sdiagnostic("%s: Element 0 is IOK", prefix));
        is(SvIV(*elem1), -9999LL, sdiagnostic("%s: Element 0 value correct", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(SvIOK(*elem2), sdiagnostic("%s: Element 1 is IOK", prefix));
        is(SvIV(*elem2), 8888LL, sdiagnostic("%s: Element 1 value correct", prefix));
    }
}

const upb_to_sv_test_case sfixed64_upb_to_sv_test_cases[] = {
    {"optional_sfixed64", "sfixed64", kUpb_FieldType_SFixed64, set_sfixed64_val, check_sv_sfixed64_val, 2},
    {"repeated_sfixed64", "repeated sfixed64", kUpb_FieldType_SFixed64, set_repeated_sfixed64_val, check_sv_repeated_sfixed64_val, 7},
    {NULL}
};

// sv_to_upb
SV* create_sv_sfixed64_val(pTHX) { return newSViv(-543210987654321LL); }
void check_upb_sfixed64_val(const upb_MessageValue *val, const char *prefix) {
    is(val->int64_val, -543210987654321LL, sdiagnostic("%s: sfixed64 value correct", prefix));
}
const sv_to_upb_test_case sfixed64_sv_to_upb_test_cases[] = {
    {"optional_sfixed64", "sfixed64", kUpb_FieldType_SFixed64, create_sv_sfixed64_val, check_upb_sfixed64_val, 1},
    {NULL}
};
