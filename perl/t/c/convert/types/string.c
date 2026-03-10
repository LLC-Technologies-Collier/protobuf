#include "t/c/convert/types/string.h"
#include "t/c/upb-perl-test.h"
#include "upb/base/string_view.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>

// test_num is external from the main runner
extern int test_num;

void set_string_hello(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->str_val = upb_StringView_FromString("hello upb"); }

void check_sv_string_hello(pTHX_ SV *sv, const char *prefix) {
    ok(SvPOK(sv), sdiagnostic("%s: SV is POK", prefix));
    is_string(SvPV_nolen(sv), "hello upb", sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_string_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_String);
    upb_MessageValue msg_val;

    msg_val.str_val = upb_StringView_FromString("repeat one");
    upb_Array_Append(arr, msg_val, arena);

    msg_val.str_val = upb_StringView_FromString("repeat two");
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_string_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvROK(sv), sdiagnostic("%s: SV is a reference", prefix));
    if (!SvROK(sv)) return;
    SV *deref = SvRV(sv);
    ok(SvTYPE(deref) == SVt_PVAV, sdiagnostic("%s: Dereferenced SV is an ARRAY", prefix));
    if (SvTYPE(deref) != SVt_PVAV) return;

    AV *av = (AV*)deref;
    is(av_len(av) + 1, 2, sdiagnostic("%s: Array has 2 elements", prefix));
    SV **elem1 = av_fetch(av, 0, 0);
    ok(elem1 && *elem1, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem1 && *elem1) {
        ok(SvPOK(*elem1), sdiagnostic("%s: Element 0 is POK", prefix));
        is_string(SvPV_nolen(*elem1), "repeat one", sdiagnostic("%s: Element 0 value correct", prefix));
    }
    SV **elem2 = av_fetch(av, 1, 0);
    ok(elem2 && *elem2, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem2 && *elem2) {
        ok(SvPOK(*elem2), sdiagnostic("%s: Element 1 is POK", prefix));
        is_string(SvPV_nolen(*elem2), "repeat two", sdiagnostic("%s: Element 1 value correct", prefix));
    }
}

const upb_to_sv_test_case string_upb_to_sv_test_cases[] = {
    {"repeated_string", "repeated string", kUpb_FieldType_String, set_repeated_string_val, check_sv_repeated_string_val, 9},
    {NULL} // Terminator
};

// sv_to_upb cases
SV* create_sv_string_hello(pTHX) { return newSVpvn("hello world", 11); }

void check_upb_string_hello(const upb_MessageValue *val, const char *prefix) {
    is_string_view(val->str_val, "hello world", 11, sdiagnostic("%s: string value correct", prefix));
}

static SV* create_sv_repeated_string(pTHX) {
    AV *av = newAV();
    av_push(av, newSVpvn("rep one", 7));
    av_push(av, newSVpvn("rep two", 7));
    return newRV_inc((SV*)av);
}

static void check_upb_repeated_string(const upb_MessageValue *val, const char *prefix) {
    const upb_Array *arr = val->array_val;
    is(upb_Array_Size(arr), 2, sdiagnostic("%s: array size", prefix));
    is_string_view(upb_Array_Get(arr, 0).str_val, "rep one", 7, sdiagnostic("%s: element 0", prefix));
    is_string_view(upb_Array_Get(arr, 1).str_val, "rep two", 7, sdiagnostic("%s: element 1", prefix));
}

const sv_to_upb_test_case string_sv_to_upb_test_cases[] = {
    {"repeated_string", "repeated string", kUpb_FieldType_String, create_sv_repeated_string, check_upb_repeated_string, 3},
    {NULL} // Terminator
};
