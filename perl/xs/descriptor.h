#ifndef PERL_PROTOBUF_DESCRIPTOR_H_
#define PERL_PROTOBUF_DESCRIPTOR_H_

#include "perl/xs/descriptor/base.h"
#include "perl/xs/descriptor/message.h"
#include "perl/xs/descriptor/enum.h"
#include "perl/xs/descriptor/enum_value.h"
#include "perl/xs/descriptor/field.h"
#include "perl/xs/descriptor/file.h"
#include "perl/xs/descriptor/method.h"
#include "perl/xs/descriptor/oneof.h"
#include "perl/xs/descriptor/service.h"

// Top-level init
bool PerlUpb_InitDescriptor(pTHX_ SV* module);

#endif // PERL_PROTOBUF_DESCRIPTOR_H_