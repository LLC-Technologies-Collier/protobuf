#ifndef PERL_PROTOBUF_EXTENSION_DICT_ITERATOR_H_
#define PERL_PROTOBUF_EXTENSION_DICT_ITERATOR_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Iterator for ExtensionDict
SV* PerlUpb_ExtensionDict_GetIterator(pTHX_ SV* dict_sv);

#endif // PERL_PROTOBUF_EXTENSION_DICT_ITERATOR_H_
