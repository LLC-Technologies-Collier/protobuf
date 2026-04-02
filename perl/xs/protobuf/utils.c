#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/utils.h"

const char* PerlUpb_GetStrData(pTHX_ SV *sv) {
    if (!sv || !SvPOK(sv)) {
        return NULL;
    }
    STRLEN len;
    return SvPV(sv, len);
}

const char* PerlUpb_VerifyStrData(pTHX_ SV *sv) {
    if (!sv || !SvPOK(sv)) {
        croak("Expected a string SV");
    }
    STRLEN len;
    return SvPV(sv, len);
}

char* PerlUpb_ClassNameToFullName(pTHX_ const char* class_name) {
    char* full_name = savepv(class_name);
    for (char* p = full_name; *p; p++) {
        if (*p == ':' && *(p+1) == ':') {
            *p = '.';
            memmove(p+1, p+2, strlen(p+2) + 1);
        }
    }
    return full_name;
    }

    void PerlUpb_Error_Die(pTHX_ const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcroak(fmt, &args);
    va_end(args);
    }