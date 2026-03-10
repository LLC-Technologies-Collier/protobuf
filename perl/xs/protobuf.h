#ifndef PERL_PROTOBUF_H_
#define PERL_PROTOBUF_H_

#include "EXTERN.h"
#include "perl.h"
#include "upb/reflection/def.h" // Keep for now, might be needed by other components
#include "upb/wire/types.h"

#include "perl/xs/protobuf/arena.h"
#include "perl/xs/protobuf/obj_cache.h"
#include "perl/xs/protobuf/utils.h"

// Top-level module initialization
void protobuf_init_module(pTHX);

SV* get_descriptor_proto_fds(void); // Remains here for now

#endif // PERL_PROTOBUF_H_