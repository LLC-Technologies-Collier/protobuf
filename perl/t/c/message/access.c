#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
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
    SV* mdef_sv = PerlUpb_MessageDef_GetWrapper(aTHX_ mdef);
    SV* msg_sv = PerlUpb_Message_NewMessage(aTHX_ mdef_sv);

    const upb_FieldDef *int_field = upb_MessageDef_FindFieldByName(mdef, "optional_int32");
    ok(int_field != NULL, "Found optional_int32 field");

    // Has (initially false)
    ok(!PerlUpb_Message_HasField(aTHX_ msg_sv, int_field), "Initially does not have field");

    // Set
    SV* val_sv = newSViv(42);
    PerlUpb_Message_SetField(aTHX_ msg_sv, int_field, val_sv);
    ok(PerlUpb_Message_HasField(aTHX_ msg_sv, int_field), "Has field after setting");

    // Get
    SV* ret_sv = PerlUpb_Message_GetField(aTHX_ msg_sv, int_field);
    ok(SvIOK(ret_sv) && SvIV(ret_sv) == 42, "Get returns correct value");
    SvREFCNT_dec(ret_sv);

    // ClearField
    PerlUpb_Message_ClearField(aTHX_ msg_sv, int_field);
    ok(!PerlUpb_Message_HasField(aTHX_ msg_sv, int_field), "Field cleared");

    // Set again
    PerlUpb_Message_SetField(aTHX_ msg_sv, int_field, val_sv);
    ok(PerlUpb_Message_HasField(aTHX_ msg_sv, int_field), "Field set again");

    // Clear message
    PerlUpb_Message_Clear(aTHX_ msg_sv);
    ok(!PerlUpb_Message_HasField(aTHX_ msg_sv, int_field), "Message cleared completely");
    ok(1, "End of tests");

    SvREFCNT_dec(val_sv);
    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ msg_sv));
    PerlUpb_Message_Free(aTHX_ msg_sv);
    SvREFCNT_dec(msg_sv);
    SvREFCNT_dec(mdef_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
