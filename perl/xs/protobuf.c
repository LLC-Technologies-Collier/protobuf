#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf.h"

// Initialize all sub-components
void protobuf_init_module(pTHX) {
    protobuf_init_obj_cache(aTHX);
    // Other initializations if needed
}

// get_descriptor_proto_fds() function remains here for now
// ... its implementation ...