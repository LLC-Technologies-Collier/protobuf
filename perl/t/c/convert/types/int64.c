#include "t/c/convert/types/int64.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>

// test_num is external from the main runner
extern int test_num;

void set_int64_max(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->int64_val = INT64_MAX; }

void check_sv_int64_max(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), INT64_MAX, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_int64_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Int64);
    upb_MessageValue msg_val;

    msg_val.int64_val = INT64_MIN;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.int64_val = 0;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.int64_val = INT64_MAX;
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_int64_val(pTHX_ SV *sv, const char *prefix) {
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
        is(SvIV(*elem1), INT64_MIN, sdiagnostic("%s: Element 0 value correct", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(SvIOK(*elem2), sdiagnostic("%s: Element 1 is IOK", prefix));
        is(SvIV(*elem2), 0, sdiagnostic("%s: Element 1 value correct", prefix));
    }
    SV **elem3 = av_fetch(av, 2, 0);
    ok(elem3 && *elem3, sdiagnostic("%s: Fetched element 2", prefix));
    if (elem3 && *elem3) {
        ok(SvIOK(*elem3), sdiagnostic("%s: Element 2 is IOK", prefix));
        is(SvIV(*elem3), INT64_MAX, sdiagnostic("%s: Element 2 value correct", prefix));
    }
}

const upb_to_sv_test_case int64_upb_to_sv_test_cases[] = {
    {"optional_int64", "int64", kUpb_FieldType_Int64, set_int64_max, check_sv_int64_max, 2},
    {"repeated_int64", "repeated int64", kUpb_FieldType_Int64, set_repeated_int64_val, check_sv_repeated_int64_val, 8},
    {NULL} // Terminator
};

// sv_to_upb cases
SV* create_sv_int64_max(pTHX) { return newSViv(INT64_MAX); }

void check_upb_int64_max(const upb_MessageValue *val, const char *prefix) {
    is(val->int64_val, INT64_MAX, sdiagnostic("%s: int64 value correct", prefix));
}

const sv_to_upb_test_case int64_sv_to_upb_test_cases[] = {
    {"optional_int64", "int64", kUpb_FieldType_Int64, create_sv_int64_max, check_upb_int64_max, 1},
    {NULL} // Terminator
};
