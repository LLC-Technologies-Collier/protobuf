#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "xs/protobuf.h"

MODULE = Protobuf  PACKAGE = Protobuf

PROTOTYPES: ENABLE

BOOT:
    PerlUpb_DetectCpuFeatures();
    PerlUpb_Protobuf_InitModule(aTHX);
