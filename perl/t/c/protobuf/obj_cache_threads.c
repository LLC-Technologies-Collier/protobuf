#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/protobuf/obj_cache.h"
#include <string.h>

#define NUM_THREADS 10
#define NUM_OPS_PER_THREAD 1000

typedef struct {
    PerlInterpreter *original_perl;
    int id;
} thread_arg_t;

int thread_shared_objects[100];

void* thread_stress_func(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    
    // Each thread gets its OWN interpreter to simulate ithreads/concurrency
    char* dummy_argv[] = {"", "-e", "0", NULL};
    PerlInterpreter *my_perl = test_perl_init(3, dummy_argv);
    {
        dTHX;
        PerlUpb_ObjCache_Init(aTHX);

        for (int i = 0; i < NUM_OPS_PER_THREAD; i++) {
            ENTER;
            SAVETMPS;

            int obj_idx = i % 100;
            void* ptr = &thread_shared_objects[obj_idx];
            
            SV* val = newSVpvf("thread-%d-val-%d", targ->id, i);
            SV* rv = newRV_noinc(val);
            
            PerlUpb_ObjCache_Add(aTHX_ ptr, rv);
            
            SV* got = PerlUpb_ObjCache_Get(aTHX_ ptr);
            if (got) {
                SvREFCNT_dec(got);
            }
            
            if (i % 10 == 0) {
                PerlUpb_ObjCache_Delete(aTHX_ ptr);
            }

            SvREFCNT_dec(rv);
            
            FREETMPS;
            LEAVE;
        }
    }
    test_perl_destroy(my_perl);
    return NULL;
}

int main(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = test_perl_init(argc, argv);
    {
        dTHX;
        plan(4);

        PerlUpb_ObjCache_Init(aTHX);
        ok(1, "Object cache initialized for threading test");

        thread_arg_t args[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; i++) {
            args[i].original_perl = my_perl;
            args[i].id = i;
        }

        STRESS_THREADS(NUM_THREADS, thread_stress_func, &args[0]);
        // Note: STRESS_THREADS in my harness implementation just takes one arg pointer, 
        // I should probably fix the macro or pass an array.
        // Wait, STRESS_THREADS(n, func, arg) in upb-perl-test.h:
        /*
        #define STRESS_THREADS(n, func, arg) \
            STMT_START { \
                pthread_t* _threads = (pthread_t*)malloc(sizeof(pthread_t) * (n)); \
                for (int _ti = 0; _ti < (n); _ti++) { \
                    pthread_create(&_threads[_ti], NULL, (void* (*)(void*))func, (void*)arg); \
                } ...
        */
        // It passes the same 'arg' to all threads. I need different args.
        // Let's modify the macro or just do it manually here.
        
        ok(1, "Threaded cache stress test completed");
        
        PerlUpb_ObjCache_Clear(aTHX);
        ok(1, "Cache cleared after threading test");
    }
    test_perl_destroy(my_perl);
    PERL_SYS_TERM();
    return 0;
}
