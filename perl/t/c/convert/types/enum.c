#include "t/c/convert/types/enum.h"
#include "t/c/upb-perl-test.h"
#include "upb/mem/arena.h"
#include "upb/message/array.h"
#include <stdint.h>
#include "xs/repeated/repeated.h"

// test_num is external from the main runner
extern int test_num;

void set_enum_bar(upb_MessageValue *val, upb_Arena *arena) { (void)arena; val->int32_val = 2; }

void check_sv_enum_bar(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), 2, sdiagnostic("%s: SV value correct", prefix));
}

static void set_repeated_enum_val(upb_MessageValue *val, upb_Arena *arena) {
    upb_Array *arr = upb_Array_New(arena, kUpb_CType_Int32);
    upb_MessageValue msg_val;

    msg_val.int32_val = 1; // FOO
    upb_Array_Append(arr, msg_val, arena);

    msg_val.int32_val = 2; // BAR
    upb_Array_Append(arr, msg_val, arena);

    val->array_val = arr;
}

static void check_sv_repeated_enum_val(pTHX_ SV *sv, const char *prefix) {
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

const upb_to_sv_test_case enum_upb_to_sv_test_cases[] = {
    {"optional_nested_enum", "optional enum", kUpb_FieldType_Enum, set_enum_bar, check_sv_enum_bar, 2},
    {"repeated_nested_enum", "repeated enum", kUpb_FieldType_Enum, set_repeated_enum_val, check_sv_repeated_enum_val, 7},
    {NULL} // Terminator
};

// sv_to_upb cases
SV* create_sv_enum_bar(pTHX) { return newSViv(2); }

void check_upb_enum_bar(const upb_MessageValue *val, const char *prefix) {
    is(val->int32_val, 2, sdiagnostic("%s: enum value correct", prefix));
}

const sv_to_upb_test_case enum_sv_to_upb_test_cases[] = {
    {"optional_nested_enum", "enum", kUpb_FieldType_Enum, create_sv_enum_bar, check_upb_enum_bar, 1},
    {NULL} // Terminator
};
