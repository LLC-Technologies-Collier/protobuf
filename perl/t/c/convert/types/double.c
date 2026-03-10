#include "t/c/convert/types/double.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>
#include <math.h>

// test_num is external from the main runner
extern int test_num;

void set_double_789_123(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->double_val = 789.123456789; }

void check_sv_double_789_123(pTHX_ SV *sv, const char *prefix) {
    ok(SvNOK(sv), sdiagnostic("%s: SV is NOK", prefix));
    ok(fabs(SvNV(sv) - 789.123456789) < 0.000000001, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_double_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Double);
    upb_MessageValue msg_val;

    msg_val.double_val = 1.23456789;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.double_val = 9.87654321;
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_double_val(pTHX_ SV *sv, const char *prefix) {
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
        ok(SvNOK(*elem1), sdiagnostic("%s: Element 0 is NOK", prefix));
        ok(fabs(SvNV(*elem1) - 1.23456789) < 0.000000001, sdiagnostic("%s: Element 0 value correct", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(SvNOK(*elem2), sdiagnostic("%s: Element 1 is NOK", prefix));
        ok(fabs(SvNV(*elem2) - 9.87654321) < 0.000000001, sdiagnostic("%s: Element 1 value correct", prefix));
    }
}

const upb_to_sv_test_case double_upb_to_sv_test_cases[] = {
    {"optional_double", "double", kUpb_FieldType_Double, set_double_789_123, check_sv_double_789_123, 2},
    {"repeated_double", "repeated double", kUpb_FieldType_Double, set_repeated_double_val, check_sv_repeated_double_val, 7},
    {NULL} // Terminator
};

// sv_to_upb cases
SV* create_sv_double_789_123(pTHX) { return newSVnv(789.123456789); }

void check_upb_double_789_123(const upb_MessageValue *val, const char *prefix) {
    ok(fabs(val->double_val - 789.123456789) < 0.000000001, sdiagnostic("%s: double value correct", prefix));
}

static SV* create_sv_repeated_double(pTHX) {
    AV *av = newAV();
    av_push(av, newSVnv(1.23456789));
    av_push(av, newSVnv(9.87654321));
    return newRV_inc((SV*)av);
}

static void check_upb_repeated_double(const upb_MessageValue *val, const char *prefix) {
    const upb_Array *arr = val->array_val;
    is(upb_Array_Size(arr), 2, sdiagnostic("%s: array size", prefix));
    ok(fabs(upb_Array_Get(arr, 0).double_val - 1.23456789) < 0.000000001, sdiagnostic("%s: element 0", prefix));
    ok(fabs(upb_Array_Get(arr, 1).double_val - 9.87654321) < 0.000000001, sdiagnostic("%s: element 1", prefix));
}

const sv_to_upb_test_case double_sv_to_upb_test_cases[] = {
    {"optional_double", "double", kUpb_FieldType_Double, create_sv_double_789_123, check_upb_double_789_123, 1},
    {"repeated_double", "repeated double", kUpb_FieldType_Double, create_sv_repeated_double, check_upb_repeated_double, 3},
    {NULL} // Terminator
};
