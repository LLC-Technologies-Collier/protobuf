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

void
_install_fast_accessors(perl_class, mdef_sv)
    const char* perl_class
    SV* mdef_sv
    CODE:
        const upb_MessageDef* mdef = PerlUpb_MessageDef_GetMessage(aTHX_ mdef_sv);
        if (!mdef) croak("Invalid MessageDef");
        PerlUpb_InstallFastAccessors(aTHX_ perl_class, mdef);
