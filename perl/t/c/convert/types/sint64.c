#include "t/c/convert/types/sint64.h"
#include "t/c/upb-perl-test.h"
#include "t/c/convert/test_util.h" // Add this
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>

// test_num is external from the main runner
extern int test_num;

// upb_to_sv
void set_sint64_val(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->int64_val = -64000000000LL; }
void check_sv_sint64_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), -64000000000LL, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_sint64_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Int64);
    upb_MessageValue msg_val;
    msg_val.int64_val = -12345678912345LL;
    upb_Array_Append(arr, msg_val, arena);
    msg_val.int64_val = 0LL;
    upb_Array_Append(arr, msg_val, arena);
    msg_val.int64_val = 98765432198765LL;
    upb_Array_Append(arr, msg_val, arena);
    val->array_val = arr;
}

static void check_sv_repeated_sint64_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvROK(sv), sdiagnostic("%s: SV is a reference", prefix));
    if (!SvROK(sv)) return;
    SV *deref = SvRV(sv);
    ok(SvTYPE(deref) == SVt_PVAV, sdiagnostic("%s: Dereferenced SV is an ARRAY", prefix));
    if (SvTYPE(deref) != SVt_PVAV) return;

    AV *av = (AV*)deref;
    is(av_len(av), 2, sdiagnostic("%s: Array has 3 elements", prefix));
    SV **elem1 = av_fetch(av, 0, 0);
    ok(elem1 && *elem1, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem1 && *elem1) {
        ok(SvIOK(*elem1), sdiagnostic("%s: Element 0 is IOK", prefix));
        is(SvIV(*elem1), -12345678912345LL, sdiagnostic("%s: Element 0 value correct", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(SvIOK(*elem2), sdiagnostic("%s: Element 1 is IOK", prefix));
        is(SvIV(*elem2), 0LL, sdiagnostic("%s: Element 1 value correct", prefix));
    }
    SV **elem3 = av_fetch(av, 2, 0);
    ok(elem3 && *elem3, sdiagnostic("%s: Fetched element 2", prefix));
    if (elem3 && *elem3) {
        ok(SvIOK(*elem3), sdiagnostic("%s: Element 2 is IOK", prefix));
        is(SvIV(*elem3), 98765432198765LL, sdiagnostic("%s: Element 2 value correct", prefix));
    }
}

const upb_to_sv_test_case sint64_upb_to_sv_test_cases[] = {
    {"optional_sint64", "sint64", kUpb_FieldType_SInt64, set_sint64_val, check_sv_sint64_val, 2},
    {"repeated_sint64", "repeated sint64", kUpb_FieldType_SInt64, set_repeated_sint64_val, check_sv_repeated_sint64_val, 8},
    {NULL}
};

// sv_to_upb
SV* create_sv_sint64_val(pTHX) { return newSViv(-128000000000LL); }

void check_upb_sint64_val(const upb_MessageValue *val, const char *prefix) {
    is(val->int64_val, -128000000000LL, sdiagnostic("%s: sint64 value correct", prefix));
}
const sv_to_upb_test_case sint64_sv_to_upb_test_cases[] = {
    {"optional_sint64", "sint64", kUpb_FieldType_SInt64, create_sv_sint64_val, check_upb_sint64_val, 1},
    {NULL}
};
