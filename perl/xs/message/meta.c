#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/message/meta.h"
#include "xs/protobuf/message.h"
#include "xs/descriptor/message.h"

SV* PerlUpb_Message_GetDescriptor(pTHX_ SV* message_sv) {
    const upb_MessageDef* mdef = PerlUpb_Message_GetDef(aTHX_ message_sv);
    if (!mdef) return &PL_sv_undef;
    return PerlUpb_MessageDef_GetWrapper(aTHX_ mdef);
}
