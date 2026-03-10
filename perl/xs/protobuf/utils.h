#ifndef PERL_PROTOBUF_UTILS_H_
#define PERL_PROTOBUF_UTILS_H_

#include "EXTERN.h"
#include "perl.h"

// String utils
const char* PerlUpb_GetStrData(pTHX_ SV *sv);
const char* PerlUpb_VerifyStrData(pTHX_ SV *sv);

#endif // PERL_PROTOBUF_UTILS_H_