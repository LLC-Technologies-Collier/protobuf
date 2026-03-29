#ifndef PERL_PROTOBUF_MESSAGE_SERIALIZE_H_
#define PERL_PROTOBUF_MESSAGE_SERIALIZE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Parses a serialized string into a new Protobuf::Message wrapper.
SV* PerlUpb_Message_Parse(pTHX_ SV* descriptor_sv, SV* data_sv);

// Serializes a Protobuf::Message wrapper into a string SV.
SV* PerlUpb_Message_Serialize(pTHX_ SV* message_sv);

#endif // PERL_PROTOBUF_MESSAGE_SERIALIZE_H_
