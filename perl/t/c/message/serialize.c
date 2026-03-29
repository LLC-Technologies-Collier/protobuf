#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/message/serialize.h"
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

    plan(7);

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

    // Set
    SV* val_sv = newSViv(12345);
    PerlUpb_Message_SetField(aTHX_ msg_sv, int_field, val_sv);
    ok(1, "Field set");

    // Serialize
    SV* serialized_sv = PerlUpb_Message_Serialize(aTHX_ msg_sv);
    ok(serialized_sv != NULL && SvPOK(serialized_sv), "Serialized correctly");

    // Parse
    SV* new_msg_sv = PerlUpb_Message_Parse(aTHX_ mdef_sv, serialized_sv);
    ok(new_msg_sv != NULL, "Parsed correctly");

    // Check value
    SV* ret_sv = PerlUpb_Message_GetField(aTHX_ new_msg_sv, int_field);
    ok(SvIOK(ret_sv) && SvIV(ret_sv) == 12345, "Parsed value matches");
    SvREFCNT_dec(ret_sv);

    // Equality check
    extern bool PerlUpb_Message_IsEqual(pTHX_ SV* message1_sv, SV* message2_sv);
    ok(PerlUpb_Message_IsEqual(aTHX_ msg_sv, new_msg_sv), "Messages are equal");
    ok(1, "End of tests");

    SvREFCNT_dec(val_sv);
    SvREFCNT_dec(serialized_sv);
    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ msg_sv));
    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ new_msg_sv));
    PerlUpb_Message_Free(aTHX_ msg_sv);
    PerlUpb_Message_Free(aTHX_ new_msg_sv);
    SvREFCNT_dec(msg_sv);
    SvREFCNT_dec(new_msg_sv);
    SvREFCNT_dec(mdef_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
