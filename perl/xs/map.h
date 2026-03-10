#ifndef PERL_MAP_H_
#define PERL_MAP_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/map/map.h"
#include "perl/xs/map/scalar.h"
#include "perl/xs/map/message.h"
#include "perl/xs/map/iterator.h"

// Top-level init
bool PerlUpb_InitMap(pTHX_ SV* module);

#endif // PERL_MAP_H_
