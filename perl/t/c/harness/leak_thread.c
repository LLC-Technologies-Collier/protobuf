#include "t/c/upb-perl-test.h"
#include <stdlib.h>

void* test_thread_func(void* arg) {
    // Just a dummy func
    return NULL;
}

int main(int argc, char** argv) {
    plan(3);

    upb_Arena* arena = upb_Arena_New();

    LEAK_CHECK(arena, {
        void* p = upb_Arena_Malloc(arena, 100);
        (void)p;
    }, "Should FAIL leak check (expectedly)");

    LEAK_CHECK(arena, {
        // no allocation
    }, "Should PASS leak check");

    STRESS_THREADS(4, test_thread_func, NULL);

    upb_Arena_Free(arena);
    return 0;
}
