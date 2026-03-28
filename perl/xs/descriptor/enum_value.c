#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/descriptor/enum_value.h"

const char* PerlUpb_EnumValueDef_Name(pTHX_ const upb_EnumValueDef *ev) {
    return upb_EnumValueDef_Name(ev);
}

int32_t PerlUpb_EnumValueDef_Number(pTHX_ const upb_EnumValueDef *ev) {
    return upb_EnumValueDef_Number(ev);
}

int PerlUpb_EnumValueDef_Index(pTHX_ const upb_EnumValueDef *ev) {
    return upb_EnumValueDef_Index(ev);
}
