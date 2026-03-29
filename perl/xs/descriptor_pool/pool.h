#ifndef PERL_PROTOBUF_DESCRIPTOR_POOL_POOL_H_
#define PERL_PROTOBUF_DESCRIPTOR_POOL_POOL_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"
#include "upb/reflection/def.h"

// Creates a NEW upb_DefPool and returns its Perl wrapper.
SV* PerlUpb_DescriptorPool_New(pTHX);

// Returns a Perl wrapper for a upb_DefPool. 
// Uses the object cache.
SV* PerlUpb_DescriptorPool_GetWrapper(pTHX_ const upb_DefPool* pool);

// Returns the underlying upb_DefPool from a Perl wrapper.
const upb_DefPool* PerlUpb_DescriptorPool_GetPool(pTHX_ SV* sv);

// Returns the singleton generated pool wrapper.
SV* PerlUpb_DescriptorPool_GeneratedPool(pTHX);

#endif // PERL_PROTOBUF_DESCRIPTOR_POOL_POOL_H_
