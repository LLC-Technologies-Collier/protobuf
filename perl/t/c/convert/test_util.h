#ifndef PERL_PROTOBUF_CONVERT_TEST_UTIL_H_
#define PERL_PROTOBUF_CONVERT_TEST_UTIL_H_

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"
#include "upb/wire/types.h"

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

extern upb_DefPool *test_pool;

typedef void (*set_upb_val_func_t)(upb_MessageValue *val, upb_Arena *arena);
typedef void (*check_sv_func_t)(pTHX_ SV *sv, const char *prefix);

typedef struct {
    const char *field_name;
    const char *test_prefix;
    upb_FieldType expected_type;
    set_upb_val_func_t set_val;
    check_sv_func_t check_sv;
    int num_checks;
} upb_to_sv_test_case;

typedef void (*check_upb_func_t)(const upb_MessageValue *val, const char *prefix);

typedef struct {
    const char *field_name;
    const char *test_prefix;
    upb_FieldType expected_type;
    SV *(*sv_creator)(pTHX);
    check_upb_func_t check_func;
    int num_checks;
} sv_to_upb_test_case;

bool load_test_descriptors(pTHX_ upb_Arena *arena);
const upb_FieldDef* get_field_def(const char *msg_name, const char *field_name);

#endif // PERL_PROTOBUF_CONVERT_TEST_UTIL_H_
