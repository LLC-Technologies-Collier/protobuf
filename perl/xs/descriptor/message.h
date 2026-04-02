#ifndef PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_
#define PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_

#include "EXTERN.h"
#include "perl.h"
#include "perl/xs/descriptor/base.h"
#include "upb/reflection/def.h"

const upb_FieldDef* PerlUpb_MessageDef_FindFieldByNameWithSize(pTHX_ const upb_MessageDef *m, const char *name, size_t len);

SV* PerlUpb_MessageDef_GetWrapper(pTHX_ const upb_MessageDef *m);
const upb_MessageDef* PerlUpb_MessageDef_GetMessage(pTHX_ SV *sv);

// Returns the full name of the message as a Perl SV
SV* PerlUpb_MessageDef_FullName(pTHX_ const upb_MessageDef *m);

#endif // PERL_PROTOBUF_DESCRIPTOR_MESSAGE_H_
