#ifndef PERL_DESCRIPTOR_CONTAINERS_H_
#define PERL_DESCRIPTOR_CONTAINERS_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

#include "perl/xs/descriptor_containers/by_name_map.h"
#include "perl/xs/descriptor_containers/by_number_map.h"
#include "perl/xs/descriptor_containers/generic_sequence.h"
#include "perl/xs/descriptor_containers/iterators.h"

// Top-level init
bool PerlUpb_InitDescriptorContainers(pTHX_ SV* module);

#endif // PERL_DESCRIPTOR_CONTAINERS_H_
