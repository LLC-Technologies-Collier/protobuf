#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/protobuf/obj_cache.h"
#include <string.h>

static void test_cache(pTHX) {
    plan(13);

    // Initialize cache
    PerlUpb_ObjCache_Init(aTHX);
    ok(1, "Cache initialized");

    int dummy1 = 1;
    int dummy2 = 2;
    const void *ptr1 = &dummy1;
    const void *ptr2 = &dummy2;

    SV *sv1 = newSVpv("value1", 0);
    SV *rv1 = newRV_noinc(sv1);
    
    SV *sv2 = newSVpv("value2", 0);
    SV *rv2 = newRV_noinc(sv2);

    // Test Add and Get
    PerlUpb_ObjCache_Add(aTHX_ ptr1, rv1);
    SV *retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr1);
    ok(retrieved_rv != NULL, "Get OK for ptr1");
    is_string(SvPV_nolen(SvRV(retrieved_rv)), "value1", "Retrieved value matches for ptr1");
    SvREFCNT_dec(retrieved_rv);

    // Test Get non-existent
    SV *non_rv = PerlUpb_ObjCache_Get(aTHX_ ptr2);
    ok(non_rv == NULL, "Get non-existent ptr2 returns NULL");

    // Test weak reference
    SvREFCNT_dec(rv1);
    // rv1 should now be gone from Perl, making the cache entry stale
    SV *retrieved_rv2 = PerlUpb_ObjCache_Get(aTHX_ ptr1);
    ok(retrieved_rv2 == NULL, "Get ptr1 after weak ref destroyed returns NULL");

    // Test re-add ptr1
    SV *sv1_new = newSVpv("new_value1", 0);
    SV *rv1_new = newRV_noinc(sv1_new);
    PerlUpb_ObjCache_Add(aTHX_ ptr1, rv1_new);
    retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr1);
    ok(retrieved_rv != NULL, "Get ptr1 after re-add OK");
    is_string(SvPV_nolen(SvRV(retrieved_rv)), "new_value1", "Re-added value matches for ptr1");
    SvREFCNT_dec(retrieved_rv);
    SvREFCNT_dec(rv1_new);

    // Add second item ptr2
    PerlUpb_ObjCache_Add(aTHX_ ptr2, rv2);
    SV *retrieved_rv3 = PerlUpb_ObjCache_Get(aTHX_ ptr2);
    ok(retrieved_rv3 != NULL, "Get second item ptr2 OK");
    is_string(SvPV_nolen(SvRV(retrieved_rv3)), "value2", "Second item value matches for ptr2");
    SvREFCNT_dec(retrieved_rv3);

    // Test Delete ptr1
    PerlUpb_ObjCache_Delete(aTHX_ ptr1);
    retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr1);
    ok(retrieved_rv == NULL, "Get ptr1 after delete returns NULL");

    // Check ptr2 still exists
    retrieved_rv3 = PerlUpb_ObjCache_Get(aTHX_ ptr2);
    ok(retrieved_rv3 != NULL, "Get ptr2 still OK after ptr1 delete");
    is_string(SvPV_nolen(SvRV(retrieved_rv3)), "value2", "Ptr2 value still correct");
    SvREFCNT_dec(retrieved_rv3);

    SvREFCNT_dec(rv2); // Clean up sv2

    // Final check on delete for ptr2
    PerlUpb_ObjCache_Delete(aTHX_ ptr2);
    retrieved_rv = PerlUpb_ObjCache_Get(aTHX_ ptr2);
    ok(retrieved_rv == NULL, "Get ptr2 after delete returns NULL");

    return;
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    test_cache(my_perl);

    test_perl_destroy(my_perl);
    return 0;
}
