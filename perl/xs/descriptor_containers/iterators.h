#ifndef PERL_PROTOBUF_DESCRIPTOR_CONTAINERS_ITERATORS_H_
#define PERL_PROTOBUF_DESCRIPTOR_CONTAINERS_ITERATORS_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

typedef struct {
  SV* container_sv;
  int index;
} PerlUpb_MapIterator;

SV* PerlUpb_MapIterator_New(pTHX_ SV* container_sv);
PerlUpb_MapIterator* PerlUpb_MapIterator_Get(pTHX_ SV* sv);

// Returns next key/value pair as a list (or undef if done)
// These will be used by the Perl-level iterator methods
SV* PerlUpb_MapIterator_NextKey(pTHX_ SV* self);
SV* PerlUpb_MapIterator_NextValue(pTHX_ SV* self);

#endif // PERL_PROTOBUF_DESCRIPTOR_CONTAINERS_ITERATORS_H_
