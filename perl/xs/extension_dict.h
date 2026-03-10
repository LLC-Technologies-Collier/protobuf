#ifndef PERL_EXTENSION_DICT_H_
#define PERL_EXTENSION_DICT_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/extension_dict/dict.h"
#include "perl/xs/extension_dict/iterator.h"

// Top-level init
bool PerlUpb_InitExtensionDict(pTHX_ SV* module);

#endif // PERL_EXTENSION_DICT_H_
