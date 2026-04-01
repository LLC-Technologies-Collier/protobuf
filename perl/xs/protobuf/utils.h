#ifndef PERL_PROTOBUF_UTILS_H_
#define PERL_PROTOBUF_UTILS_H_

#include "EXTERN.h"
#include "perl.h"

// String utils
const char* PerlUpb_GetStrData(pTHX_ SV *sv);
const char* PerlUpb_VerifyStrData(pTHX_ SV *sv);

// Converts a Perl class name (A::B) to a Protobuf full name (A.B).
// Caller is responsible for Safefree()ing the returned string.
char* PerlUpb_ClassNameToFullName(pTHX_ const char* class_name);

#endif // PERL_PROTOBUF_UTILS_H_