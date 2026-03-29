#ifndef PERL_PROTOBUF_MESSAGE_META_H_
#define PERL_PROTOBUF_MESSAGE_META_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Returns the descriptor for a message wrapper
SV* PerlUpb_Message_GetDescriptor(pTHX_ SV* message_sv);

#endif // PERL_PROTOBUF_MESSAGE_META_H_
