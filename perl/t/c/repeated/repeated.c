#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/repeated/repeated.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
#include "upb/message/array.h"
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

    plan(9);

    extern void PerlUpb_ObjCache_Init(pTHX);
    PerlUpb_ObjCache_Init(aTHX);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) return 1;
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    const upb_FieldDef *rep_int32_field = upb_MessageDef_FindFieldByName(mdef, "repeated_int32");
    ok(rep_int32_field != NULL, "Found repeated_int32 field");

    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    upb_Array* arr = upb_Message_Mutable(msg, rep_int32_field, arena).array;
    ok(arr != NULL, "Created upb_Array");

    SV* rep_sv = PerlUpb_Repeated_New(aTHX_ arr, rep_int32_field, arena_sv);
    ok(rep_sv != NULL && sv_derived_from(rep_sv, "Protobuf::Repeated"), "Created Protobuf::Repeated wrapper");

    // 1. Append
    PerlUpb_Repeated_Append(aTHX_ rep_sv, newSViv(10));
    PerlUpb_Repeated_Append(aTHX_ rep_sv, newSViv(20));
    is(PerlUpb_Repeated_Size(aTHX_ rep_sv), 2, "Size is 2");

    // 2. Get
    SV* v0 = PerlUpb_Repeated_GetItem(aTHX_ rep_sv, 0);
    is(SvIV(v0), 10, "Item 0 is 10");
    SvREFCNT_dec(v0);

    // 3. Set
    PerlUpb_Repeated_SetItem(aTHX_ rep_sv, 1, newSViv(30));
    SV* v1 = PerlUpb_Repeated_GetItem(aTHX_ rep_sv, 1);
    is(SvIV(v1), 30, "Item 1 updated to 30");
    SvREFCNT_dec(v1);

    // 4. Delete
    PerlUpb_Repeated_Delete(aTHX_ rep_sv, 0, 1);
    is(PerlUpb_Repeated_Size(aTHX_ rep_sv), 1, "Size is 1 after delete");
    
    SV* nv0 = PerlUpb_Repeated_GetItem(aTHX_ rep_sv, 0);
    is(SvIV(nv0), 30, "New item 0 is 30");
    SvREFCNT_dec(nv0);

    // Cleanup
    extern void PerlUpb_Repeated_Free(pTHX_ SV* sv);
    PerlUpb_Repeated_Free(aTHX_ rep_sv);
    SvREFCNT_dec(rep_sv);
    
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
