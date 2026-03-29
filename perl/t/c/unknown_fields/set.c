#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/message.h"
#include "xs/message/message.h"
#include "xs/unknown_fields/set.h"
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

    SV* set_sv = PerlUpb_UnknownFieldSet_New(aTHX_ msg_sv);
    ok(set_sv != NULL && sv_derived_from(set_sv, "Protobuf::UnknownFieldSet"), "Created UnknownFieldSet wrapper");

    // 1. Initially empty
    SV* data0 = PerlUpb_UnknownFieldSet_GetData(aTHX_ set_sv);
    is(SvCUR(data0), 0, "Initially no unknown fields");
    SvREFCNT_dec(data0);

    // 2. Add some raw data (tag 100, wire type 0 (varint), value 42)
    // Tag 100 << 3 | 0 = 800 (0x320)
    // 800 in varint: 0xA0 0x06
    // 42 in varint: 0x2A
    const char raw_data[] = { 0xA0, 0x06, 0x2A };
    SV* data_sv = newSVpvn(raw_data, sizeof(raw_data));
    PerlUpb_UnknownFieldSet_Add(aTHX_ set_sv, data_sv);
    ok(1, "Added unknown field raw data");
    SvREFCNT_dec(data_sv);

    // 3. Verify data
    SV* data1 = PerlUpb_UnknownFieldSet_GetData(aTHX_ set_sv);
    is(SvCUR(data1), sizeof(raw_data), "Data length matches");
    is_string(SvPV_nolen(data1), raw_data, "Data content matches");
    SvREFCNT_dec(data1);

    // 4. Clear
    PerlUpb_UnknownFieldSet_Clear(aTHX_ set_sv);
    SV* data2 = PerlUpb_UnknownFieldSet_GetData(aTHX_ set_sv);
    is(SvCUR(data2), 0, "Data cleared");
    SvREFCNT_dec(data2);

    // Cleanup
    extern void PerlUpb_UnknownFieldSet_Free(pTHX_ SV* sv);
    PerlUpb_UnknownFieldSet_Free(aTHX_ set_sv);
    SvREFCNT_dec(set_sv);

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
