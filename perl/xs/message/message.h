#ifndef PERL_PROTOBUF_MESSAGE_MESSAGE_H_
#define PERL_PROTOBUF_MESSAGE_MESSAGE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Creates a new Protobuf::Message wrapper, allocating a new upb_Message
// and upb_Arena internally.
SV* PerlUpb_Message_NewMessage(pTHX_ SV* descriptor_sv);

#endif // PERL_PROTOBUF_MESSAGE_MESSAGE_H_
