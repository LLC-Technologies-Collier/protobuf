#include "t/c/convert/types/sint32.h"
#include "t/c/upb-perl-test.h"
#include "t/c/convert/test_util.h" // Add this
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>
#include "xs/repeated/repeated.h"

// test_num is external from the main runner
extern int test_num;

// upb_to_sv
void set_sint32_val(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->int32_val = -321; }
void check_sv_sint32_val(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), -321, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_sint32_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Int32);
    upb_MessageValue msg_val;
    msg_val.int32_val = -10;
    upb_Array_Append(arr, msg_val, arena);
    msg_val.int32_val = 20;
    upb_Array_Append(arr, msg_val, arena);
    val->array_val = arr;
}

static void check_sv_repeated_sint32_val(pTHX_ SV *sv, const char *prefix) {
    ok(sv_derived_from(sv, "Protobuf::Internal::Repeated"), sdiagnostic("%s: SV is a Repeated wrapper", prefix));
    if (!sv_derived_from(sv, "Protobuf::Internal::Repeated")) return;

    int size = PerlUpb_Repeated_Size(aTHX_ sv);
    is(size, 2, sdiagnostic("%s: Array has 2 elements", prefix));
    SV *elem0 = PerlUpb_Repeated_GetItem(aTHX_ sv, 0);
    ok(elem0, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem0) {
        ok(SvPOK(elem0) || SvIOK(elem0) || SvNOK(elem0), sdiagnostic("%s: Element 0 is valid", prefix));
        SvREFCNT_dec(elem0);
    }
    SV *elem1 = PerlUpb_Repeated_GetItem(aTHX_ sv, 1);
    ok(elem1, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem1) {
        ok(SvPOK(elem1) || SvIOK(elem1) || SvNOK(elem1), sdiagnostic("%s: Element 1 is valid", prefix));
        SvREFCNT_dec(elem1);
    }
}

const upb_to_sv_test_case sint32_upb_to_sv_test_cases[] = {
    {"optional_sint32", "sint32", kUpb_FieldType_SInt32, set_sint32_val, check_sv_sint32_val, 2},
    {"repeated_sint32", "repeated sint32", kUpb_FieldType_SInt32, set_repeated_sint32_val, check_sv_repeated_sint32_val, 7},
    {NULL}
};

// sv_to_upb
SV* create_sv_sint32_val(pTHX) { return newSViv(-123); }

void check_upb_sint32_val(const upb_MessageValue *val, const char *prefix) {
    is(val->int32_val, -123, sdiagnostic("%s: sint32 value correct", prefix));
}
const sv_to_upb_test_case sint32_sv_to_upb_test_cases[] = {
    {"optional_sint32", "sint32", kUpb_FieldType_SInt32, create_sv_sint32_val, check_upb_sint32_val, 1},
    {NULL}
};
