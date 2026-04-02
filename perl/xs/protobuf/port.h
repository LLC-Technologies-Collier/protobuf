#ifndef PERL_PROTOBUF_PORT_H_
#define PERL_PROTOBUF_PORT_H_

#include "EXTERN.h"
#include "perl.h"

/* Mutex abstractions using Perl's native macros if available */
#ifdef USE_ITHREADS
#define PERL_PROTOBUF_MUTEX_T perl_mutex
#define PERL_PROTOBUF_MUTEX_INIT(m) MUTEX_INIT(m)
#define PERL_PROTOBUF_MUTEX_DESTROY(m) MUTEX_DESTROY(m)
#define PERL_PROTOBUF_MUTEX_LOCK(m) MUTEX_LOCK(m)
#define PERL_PROTOBUF_MUTEX_UNLOCK(m) MUTEX_UNLOCK(m)
#else
#define PERL_PROTOBUF_MUTEX_T int
#define PERL_PROTOBUF_MUTEX_INIT(m)
#define PERL_PROTOBUF_MUTEX_DESTROY(m)
#define PERL_PROTOBUF_MUTEX_LOCK(m)
#define PERL_PROTOBUF_MUTEX_UNLOCK(m)
#endif

#endif // PERL_PROTOBUF_PORT_H_
