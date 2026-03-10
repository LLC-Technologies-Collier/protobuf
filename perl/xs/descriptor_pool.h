#ifndef PERL_DESCRIPTOR_POOL_H_
#define PERL_DESCRIPTOR_POOL_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/descriptor_pool/pool.h"
#include "perl/xs/descriptor_pool/add.h"
#include "perl/xs/descriptor_pool/find.h"

// Top-level init
bool PerlUpb_InitDescriptorPool(pTHX_ SV* module);

#endif // PERL_DESCRIPTOR_POOL_H_