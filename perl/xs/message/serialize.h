#ifndef PERL_PROTOBUF_MESSAGE_SERIALIZE_H_
#define PERL_PROTOBUF_MESSAGE_SERIALIZE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/protobuf.h"

// Parses a serialized string into a new Protobuf::Message wrapper.
SV* PerlUpb_Message_Parse(pTHX_ SV* descriptor_sv, SV* data_sv);

// Serializes a Protobuf::Message wrapper into a string SV.
SV* PerlUpb_Message_Serialize(pTHX_ SV* message_sv);

// Serializes a Protobuf::Message wrapper into a string SV with deterministic field ordering.
SV* PerlUpb_Message_Serialize_Deterministic(pTHX_ SV* message_sv);

// Serializes a Protobuf::Message wrapper into a TextFormat string SV.
SV* PerlUpb_Message_ToText(pTHX_ SV* message_sv);

// Serializes a Protobuf::Message wrapper into a JSON string SV.
SV* PerlUpb_Message_ToJson(pTHX_ SV* message_sv);

// Parses a JSON string SV into a new Protobuf::Message wrapper.
SV* PerlUpb_Message_FromJson(pTHX_ SV* class_name, SV* json_sv);

#endif // PERL_PROTOBUF_MESSAGE_SERIALIZE_H_
