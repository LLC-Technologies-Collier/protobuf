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