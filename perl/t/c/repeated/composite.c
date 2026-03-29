#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/repeated/repeated.h"
#include "xs/repeated/composite.h"
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

    plan(7);

    extern void PerlUpb_ObjCache_Init(pTHX);
    PerlUpb_ObjCache_Init(aTHX);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) return 1;
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    const upb_FieldDef *rep_msg_field = upb_MessageDef_FindFieldByName(mdef, "repeated_nested_message");
    ok(rep_msg_field != NULL, "Found repeated_nested_message field");

    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    upb_Array* arr = upb_Message_Mutable(msg, rep_msg_field, arena).array;
    
    SV* rep_sv = PerlUpb_Repeated_New(aTHX_ arr, rep_msg_field, arena_sv);

    // 1. Add
    SV* submsg_sv = PerlUpb_Repeated_Add(aTHX_ rep_sv);
    ok(submsg_sv != NULL && sv_isobject(submsg_sv), "Add returns a message object");
    is(PerlUpb_Repeated_Size(aTHX_ rep_sv), 1, "Size is 1");

    // 2. Set field on added message
    const upb_MessageDef* sub_mdef = upb_FieldDef_MessageSubDef(rep_msg_field);
    const upb_FieldDef* f_a = upb_MessageDef_FindFieldByName(sub_mdef, "a");
    
    extern void PerlUpb_Message_SetField(pTHX_ SV* message_sv, const upb_FieldDef* f, SV* val_sv);
    PerlUpb_Message_SetField(aTHX_ submsg_sv, f_a, newSViv(42));
    
    // 3. Get and check
    SV* ret_submsg_sv = PerlUpb_Repeated_GetItem(aTHX_ rep_sv, 0);
    
    ok(ret_submsg_sv != NULL && SvROK(ret_submsg_sv) && SvROK(submsg_sv) && SvRV(ret_submsg_sv) == SvRV(submsg_sv), "GetItem returns SAME SV from cache");
    
    extern SV* PerlUpb_Message_GetField(pTHX_ SV* message_sv, const upb_FieldDef* f);
    SV* val_a = PerlUpb_Message_GetField(aTHX_ ret_submsg_sv, f_a);
    is(SvIV(val_a), 42, "Field 'a' in submessage is 42");
    SvREFCNT_dec(val_a);
    SvREFCNT_dec(ret_submsg_sv);

    // Cleanup
    SvREFCNT_dec(submsg_sv);
    extern void PerlUpb_Repeated_Free(pTHX_ SV* sv);
    PerlUpb_Repeated_Free(aTHX_ rep_sv);
    SvREFCNT_dec(rep_sv);
    
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);
    ok(1, "Clean cleanup");

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
