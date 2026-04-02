#ifndef PERL_PROTOBUF_UTILS_H_
#define PERL_PROTOBUF_UTILS_H_

#include "EXTERN.h"
#include "perl.h"

struct upb_MessageDef;
typedef struct upb_MessageDef upb_MessageDef;
struct upb_FieldDef;
typedef struct upb_FieldDef upb_FieldDef;

// String utils
const char* PerlUpb_GetStrData(pTHX_ SV *sv);
const char* PerlUpb_VerifyStrData(pTHX_ SV *sv);

// Converts a Perl class name (A::B) to a Protobuf full name (A.B).
// Caller is responsible for Safefree()ing the returned string.
char* PerlUpb_ClassNameToFullName(pTHX_ const char* class_name);

// Converts a Protobuf full name (A.B) to a Perl class name (A::B).
// Caller is responsible for Safefree()ing the returned string.
char* PerlUpb_FullNameToClassName(pTHX_ const char* full_name);

// Logs a message and dies with Perl context
void PerlUpb_Error_Die(pTHX_ const char* fmt, ...);

// Wraps a C pointer into a Perl object, optionally keeping another Perl object (the arena) alive.
SV* PerlUpb_WrapArenaBoundObject(pTHX_ const void* ptr, SV* arena_sv, const char* class_name);

// Extracts the C pointer from a wrapped object, verifying the class name.
const void* PerlUpb_GetArenaBoundObject(pTHX_ SV* sv, const char* class_name);

// Retrieves the arena SV associated with the wrapped object.
SV* PerlUpb_GetArenaFromObject(pTHX_ SV* sv);

// 64-bit numeric conversion
int64_t PerlUpb_SVToI64(pTHX_ SV* sv);
uint64_t PerlUpb_SVToU64(pTHX_ SV* sv);
SV* PerlUpb_I64ToSV(pTHX_ int64_t val);
SV* PerlUpb_U64ToSV(pTHX_ uint64_t val);

// Context-aware error reporting
void PerlUpb_CroakWithContext(pTHX_ const char* msg, const upb_MessageDef* mdef,
                             const upb_FieldDef* fdef);

#endif // PERL_PROTOBUF_UTILS_H_