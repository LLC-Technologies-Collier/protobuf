#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/map/map.h"
#include "xs/map/iterator.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
#include "upb/message/map.h"
#include "upb/reflection/message.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PERL_SYS_INIT3(&argc, &argv, &environ);
    PerlInterpreter *my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    char *embedding[] = { (char*)"", (char*)"-e", "0", NULL };
    perl_parse(my_perl, NULL, 3, embedding, NULL);
    perl_run(my_perl);

    plan(10);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) {
         fail("Failed to load test descriptors");
         return 1;
    }
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    const upb_FieldDef *map_field = upb_MessageDef_FindFieldByName(mdef, "map_int32_int32");
    ok(mdef != NULL, "Found TestAllTypesProto2");
    ok(map_field != NULL, "Found map_int32_int32");

    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    upb_MutableMessageValue mmv = upb_Message_Mutable(msg, map_field, arena);
    upb_Map* map_ptr = mmv.map;
    ok(map_ptr != NULL, "Created upb_Map");

    SV* map_sv = PerlUpb_Map_New(aTHX_ map_ptr, map_field, arena_sv);
    ok(map_sv != NULL, "Created Protobuf::Map");

    // Test SetItem
    SV* key1 = newSViv(10);
    SV* val1 = newSViv(100);
    PerlUpb_Map_SetItem(aTHX_ map_sv, key1, val1);
    is(PerlUpb_Map_Size(aTHX_ map_sv), 1, "Map size is 1");

    // Test GetItem
    SV* ret_val = PerlUpb_Map_GetItem(aTHX_ map_sv, key1);
    is(SvIV(ret_val), 100, "Retrieved value matches");
    SvREFCNT_dec(ret_val);

    // Test Iterator
    SV* iter_sv = PerlUpb_Map_GetIterator(aTHX_ map_sv);
    SV* iter_key = PerlUpb_Map_Iterator_NextKey(aTHX_ iter_sv);
    ok(SvIOK(iter_key) && SvIV(iter_key) == 10, "Iterator returned correct key");
    SV* iter_val = PerlUpb_Map_Iterator_Value(aTHX_ iter_sv);
    ok(SvIOK(iter_val) && SvIV(iter_val) == 100, "Iterator returned correct value");
    
    SvREFCNT_dec(iter_key);
    SvREFCNT_dec(iter_val);
    
    // Finalize iterator
    SV* end_key = PerlUpb_Map_Iterator_NextKey(aTHX_ iter_sv);
    ok(!SvOK(end_key), "Iterator ended");
    SvREFCNT_dec(end_key);

    // Cleanup
    extern void PerlUpb_MapIterator_Free(pTHX_ SV* sv);
    PerlUpb_MapIterator_Free(aTHX_ iter_sv);
    SvREFCNT_dec(iter_sv);

    extern void PerlUpb_Map_Free(pTHX_ SV* sv);
    PerlUpb_Map_Free(aTHX_ map_sv);
    SvREFCNT_dec(map_sv);

    SvREFCNT_dec(key1);
    SvREFCNT_dec(val1);
    
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
