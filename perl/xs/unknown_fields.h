#ifndef PERL_UNKNOWN_FIELDS_H_
#define PERL_UNKNOWN_FIELDS_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/unknown_fields/set.h"
#include "perl/xs/unknown_fields/build.h"

// Top-level init
bool PerlUpb_InitUnknownFields(pTHX_ SV* module);

#endif // PERL_UNKNOWN_FIELDS_H_
