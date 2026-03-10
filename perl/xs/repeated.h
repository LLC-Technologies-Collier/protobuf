#ifndef PERL_REPEATED_H_
#define PERL_REPEATED_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/repeated/repeated.h"
#include "perl/xs/repeated/scalar.h"
#include "perl/xs/repeated/composite.h"

// Top-level init
bool PerlUpb_InitRepeated(pTHX_ SV* module);

#endif // PERL_REPEATED_H_
