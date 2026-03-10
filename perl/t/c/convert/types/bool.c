#include "t/c/convert/types/bool.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>
#include <stdbool.h>

// test_num is external from the main runner
extern int test_num;

void set_bool_true(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->bool_val = true; }
void set_bool_false(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->bool_val = false; }

void check_sv_bool_true(pTHX_ SV *sv, const char *prefix) {
    ok(SvTRUE(sv), sdiagnostic("%s: SV is true", prefix));
}
void check_sv_bool_false(pTHX_ SV *sv, const char *prefix) {
    ok(!SvTRUE(sv), sdiagnostic("%s: SV is false", prefix));
}

static void set_repeated_bool_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Bool);
    upb_MessageValue msg_val;

    msg_val.bool_val = true;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.bool_val = false;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.bool_val = true;
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_bool_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvROK(sv), sdiagnostic("%s: SV is a reference", prefix));
    if (!SvROK(sv)) return;
    SV *deref = SvRV(sv);
    ok(SvTYPE(deref) == SVt_PVAV, sdiagnostic("%s: Dereferenced SV is an ARRAY", prefix));
    if (SvTYPE(deref) != SVt_PVAV) return;

    AV *av = (AV*)deref;
    is(av_len(av) + 1, 3, sdiagnostic("%s: Array has 3 elements", prefix));
    SV **elem1 = av_fetch(av, 0, 0);
    ok(elem1 && *elem1, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem1 && *elem1) {
        ok(SvTRUE(*elem1), sdiagnostic("%s: Element 0 is true", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(!SvTRUE(*elem2), sdiagnostic("%s: Element 1 is false", prefix));
    }
    SV **elem3 = av_fetch(av, 2, 0);
    ok(elem3 && *elem3, sdiagnostic("%s: Fetched element 2", prefix));
    if (elem3 && *elem3) {
        ok(SvTRUE(*elem3), sdiagnostic("%s: Element 2 is true", prefix));
    }
}

const upb_to_sv_test_case bool_upb_to_sv_test_cases[] = {
    {"optional_bool", "bool true", kUpb_FieldType_Bool, set_bool_true, check_sv_bool_true, 1},
    {"optional_bool", "bool false", kUpb_FieldType_Bool, set_bool_false, check_sv_bool_false, 1},
    {"repeated_bool", "repeated bool", kUpb_FieldType_Bool, set_repeated_bool_val, check_sv_repeated_bool_val, 8},
    {NULL} // Terminator
};

// sv_to_upb cases
SV* create_sv_bool_true(pTHX) { return &PL_sv_yes; }
SV* create_sv_bool_false(pTHX) { return &PL_sv_no; }

void check_upb_bool_true(const upb_MessageValue *val, const char *prefix) {
    is(val->bool_val, true, sdiagnostic("%s: bool true value correct", prefix));
}
void check_upb_bool_false(const upb_MessageValue *val, const char *prefix) {
    is(val->bool_val, false, sdiagnostic("%s: bool false value correct", prefix));
}

static SV* create_sv_repeated_bool(pTHX) {
    AV *av = newAV();
    av_push(av, newSVsv(&PL_sv_yes));
    av_push(av, newSVsv(&PL_sv_no));
    av_push(av, newSVsv(&PL_sv_yes));
    return newRV_inc((SV*)av);
}

static void check_upb_repeated_bool(const upb_MessageValue *val, const char *prefix) {
    const upb_Array *arr = val->array_val;
    is(upb_Array_Size(arr), 3, sdiagnostic("%s: array size", prefix));
    is(upb_Array_Get(arr, 0).bool_val, true, sdiagnostic("%s: element 0", prefix));
    is(upb_Array_Get(arr, 1).bool_val, false, sdiagnostic("%s: element 1", prefix));
    is(upb_Array_Get(arr, 2).bool_val, true, sdiagnostic("%s: element 2", prefix));
}

const sv_to_upb_test_case bool_sv_to_upb_test_cases[] = {
    {"optional_bool", "bool true", kUpb_FieldType_Bool, create_sv_bool_true, check_upb_bool_true, 1},
    {"optional_bool", "bool false", kUpb_FieldType_Bool, create_sv_bool_false, check_upb_bool_false, 1},
    {"repeated_bool", "repeated bool", kUpb_FieldType_Bool, create_sv_repeated_bool, check_upb_repeated_bool, 4},
    {NULL} // Terminator
};
