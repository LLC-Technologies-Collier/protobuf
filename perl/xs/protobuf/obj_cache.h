#ifndef PERL_PROTOBUF_OBJ_CACHE_H_
#define PERL_PROTOBUF_OBJ_CACHE_H_

#include "EXTERN.h"
#include "perl.h"

// Object Cache
void protobuf_init_obj_cache(pTHX);
void protobuf_register_object(pTHX_ const char *key, SV *value);
SV *protobuf_get_object(pTHX_ const char *key);
void protobuf_unregister_object(pTHX_ const char *key);

#endif // PERL_PROTOBUF_OBJ_CACHE_H_

void protobuf_clear_obj_cache(PerlInterpreter *my_perl);
