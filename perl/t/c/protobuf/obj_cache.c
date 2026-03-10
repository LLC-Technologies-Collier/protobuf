#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"

#include <string.h>

static void test_cache(pTHX) {
    plan(17);

    // Initialize cache
    protobuf_init_obj_cache(aTHX);
    ok(1, "Cache initialized");

    const char *key1 = "key1";
    const char *key2 = "key2";
    SV *sv1 = newSVpvn("value1", 6);
    SV *sv2 = newSVpvn("value2", 6);
    SV *sv1_new = newSVpvn("new_value1", 10);

    // Test Add and Get
    protobuf_register_object(aTHX_ key1, sv1);
    SV *retrieved_sv = protobuf_get_object(aTHX_ key1);
    ok(retrieved_sv != NULL, "Get OK for key1");
    is_string(SvPV_nolen(retrieved_sv), "value1", "Retrieved value matches for key1");
    SvREFCNT_dec(retrieved_sv);

    // Test Get non-existent
    SV *non_sv = protobuf_get_object(aTHX_ key2);
    ok(non_sv == NULL, "Get non-existent key2 returns NULL");

    // Test weak reference
    SvREFCNT_dec(sv1);
    // sv1 should now be gone from Perl, making the cache entry stale
    SV *retrieved_sv2 = protobuf_get_object(aTHX_ key1);
    ok(retrieved_sv2 == NULL, "Get key1 after weak ref destroyed returns NULL");

    // Test re-add key1
    protobuf_register_object(aTHX_ key1, sv1_new);
    retrieved_sv = protobuf_get_object(aTHX_ key1);
    ok(retrieved_sv != NULL, "Get key1 after re-add OK");
    is_string(SvPV_nolen(retrieved_sv), "new_value1", "Re-added value matches for key1");
    SvREFCNT_dec(retrieved_sv);
    SvREFCNT_dec(sv1_new);

    // Add second item key2
    protobuf_register_object(aTHX_ key2, sv2);
    SV *retrieved_sv3 = protobuf_get_object(aTHX_ key2);
    ok(retrieved_sv3 != NULL, "Get second item key2 OK");
    is_string(SvPV_nolen(retrieved_sv3), "value2", "Second item value matches for key2");
    SvREFCNT_dec(retrieved_sv3);

    // Test Unregister key1
    protobuf_unregister_object(aTHX_ key1);
    retrieved_sv = protobuf_get_object(aTHX_ key1);
    ok(retrieved_sv == NULL, "Get key1 after unregister returns NULL");

    // Test Unregister non-existent key
    const char *key3 = "key3";
    protobuf_unregister_object(aTHX_ key3); // Should not crash
    ok(1, "Unregister non-existent key3 did not crash");
    retrieved_sv = protobuf_get_object(aTHX_ key3);
    ok(retrieved_sv == NULL, "Get key3 remains NULL");

    // Check key2 still exists
    retrieved_sv3 = protobuf_get_object(aTHX_ key2);
    ok(retrieved_sv3 != NULL, "Get key2 still OK after key1 unregister");
    is_string(SvPV_nolen(retrieved_sv3), "value2", "Key2 value still correct");
    SvREFCNT_dec(retrieved_sv3);

    // Test Overwriting key2
    SV *sv2_overwrite = newSVpvn("overwrite2", 10);
    protobuf_register_object(aTHX_ key2, sv2_overwrite);
    retrieved_sv3 = protobuf_get_object(aTHX_ key2);
    ok(retrieved_sv3 != NULL, "Get key2 after overwrite OK");
    is_string(SvPV_nolen(retrieved_sv3), "overwrite2", "Overwritten value for key2 is correct");
    SvREFCNT_dec(retrieved_sv3);
    SvREFCNT_dec(sv2_overwrite);

    SvREFCNT_dec(sv2); // Clean up original sv2

    // Final check on unregister for key2
    protobuf_unregister_object(aTHX_ key2);
    retrieved_sv = protobuf_get_object(aTHX_ key2);
    ok(retrieved_sv == NULL, "Get key2 after unregister returns NULL");

    return;
}

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

/* for PERL_SYS_INIT, PERL_SYS_TERM */
#include <EXTERN.h>
#include <perl.h>

static PerlInterpreter *my_perl;

void xs_init(pTHX);

int main(int argc, char** argv) {
    PERL_SYS_INIT(&argc, &argv);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);

    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *argv_parse[] = { (char*)"", (char*)"-e", (char*)"0", NULL };
    int exitstatus = perl_parse(my_perl, xs_init, 3, argv_parse, (char **)NULL);
    if (exitstatus != 0) {
        fprintf(stderr, "perl_parse failed with status %d\n", exitstatus);
        return exitstatus;
    }
    perl_run(my_perl);

    test_cache(my_perl);

    perl_destruct(my_perl);
    perl_free(my_perl);
    // PERL_SYS_TERM(); // Avoid double free
    return 0;
}

void xs_init(pTHX) {
    // Intentionally empty
}
