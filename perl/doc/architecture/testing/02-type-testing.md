# Type-Specific C Tests

_Status: Planned_

To make the C-level tests for type conversions more modular and maintainable, the test logic for each individual `upb_FieldType` is encapsulated in its own file within `perl/t/c/convert/types/`.

## File Structure (`t/c/convert/types/<type>.c`)

Each `<type>.c` file (e.g., `int32.c`, `string.c`, `message.c`) should generally contain:

1.  **Setter Function(s):** Functions like `set_<type>_values(upb_MessageValue *val, ...)` to populate a `upb_MessageValue` with test data for this type.
2.  **Checker Function(s):** Functions like `check_sv_is_<type>(pTHX_ SV *sv, ...)` to assert that the Perl SV resulting from a conversion matches the expected value and type. These functions use the macros from `t/c/upb-perl-test.h` to print TAP output.
3.  **Test Case Definitions:** A static array of structs, or functions returning such, that define specific test scenarios for this type. These scenarios are then used by the main test runners (`upb_to_sv.c`, `sv_to_upb.c`).

## Example: `int32.c` (Conceptual)

```c
#include "t/c/convert/test_util.h"
#include "t/c/upb-perl-test.h"
#include "xs/convert/upb_to_sv.h" // Or sv_to_upb.h

// test_num is external from the main runner
extern int test_num;

static void set_int32_123(upb_MessageValue *val) { val->int32_val = 123; }

static void check_sv_int32_123(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), 123, sdiagnostic("%s: SV value correct", prefix));
}

const upb_to_sv_test_case int32_test_cases[] = {
    {"optional_int32", "int32", kUpb_FieldType_Int32, set_int32_123, check_sv_int32_123, 2},
    // ... more int32 test cases ...
    {NULL}
};
```

## Integration

The main test files (`t/c/convert/upb_to_sv.c` and `t/c/convert/sv_to_upb.c`) will include the necessary headers or declarations from these type files and incorporate the `*_test_cases` arrays into their main test loop. The `c_test_config.json` in `t/c/` ensures all these `.c` files are compiled and linked together.
