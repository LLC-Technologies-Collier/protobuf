#include "t/c/convert/types/uint64.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>
#include "xs/repeated/repeated.h"
#include <math.h>
#include <stdbool.h>

// test_num is external from the main runner
extern int test_num;

void set_uint64_max(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->uint64_val = UINT64_MAX; }

void check_sv_uint64(pTHX_ SV *sv, uint64_t expected_val, const char *prefix) {
    uint64_t actual_val = 0;
    bool matched = false;
    if (SvUOK(sv)) { actual_val = SvUV(sv); matched = true; }
    else if (SvIOK(sv) && SvIV(sv) >= 0) { actual_val = (uint64_t)SvIV(sv); matched = true; }
    else if (SvNOK(sv)) { double nv = SvNV(sv); if (nv >= 0 && nv <= (double)UINT64_MAX && floor(nv) == nv) { actual_val = (uint64_t)nv; matched = true; } }
    if (matched) {
        is_u(actual_val, expected_val, sdiagnostic("%s: SV value correct", prefix));
    } else {
        ok(0, sdiagnostic("%s: SV not a valid uint64 representation", prefix));
    }
}

void check_sv_uint64_max(pTHX_ SV *sv, const char *prefix) {
    check_sv_uint64(aTHX_ sv, UINT64_MAX, prefix);
}

static void set_repeated_uint64_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_UInt64);
    upb_MessageValue msg_val;

    msg_val.uint64_val = 0;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.uint64_val = 12345678912345ULL;
    upb_Array_Append(arr, msg_val, arena);

    msg_val.uint64_val = UINT64_MAX;
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_uint64_val(pTHX_ SV *sv, const char *prefix) {
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

const upb_to_sv_test_case uint64_upb_to_sv_test_cases[] = {
    {"optional_uint64", "uint64", kUpb_FieldType_UInt64, set_uint64_max, check_sv_uint64_max, 1},
    {"repeated_uint64", "repeated uint64", kUpb_FieldType_UInt64, set_repeated_uint64_val, check_sv_repeated_uint64_val, 6},
    {NULL}
};

// sv_to_upb cases
SV* create_sv_uint64_max(pTHX) { return newSVuv(UINT64_MAX); }

void check_upb_uint64_max(const upb_MessageValue *val, const char *prefix) {
    is_u(val->uint64_val, UINT64_MAX, sdiagnostic("%s: uint64 value correct", prefix));
}

const sv_to_upb_test_case uint64_sv_to_upb_test_cases[] = {
    {"optional_uint64", "uint64", kUpb_FieldType_UInt64, create_sv_uint64_max, check_upb_uint64_max, 1},
    {NULL} // Terminator
};
