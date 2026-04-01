#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/descriptor/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/message/serialize.h"
#include "xs/unknown_fields/set.h"
#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "t/c/convert/test_util.h"
#include "upb/reflection/message.h"
#include <stdio.h>

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(7);

    extern void PerlUpb_ObjCache_Init(pTHX);
    PerlUpb_ObjCache_Init(aTHX);

    SV *arena_sv = PerlUpb_Arena_New(aTHX);
    upb_Arena *arena = PerlUpb_Arena_Get(aTHX_ arena_sv);

    if (!load_test_descriptors(aTHX_ arena)) return 1;
    ok(1, "Descriptors loaded");

    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "protobuf_test_messages.proto2.TestAllTypesProto2");
    SV* mdef_sv = PerlUpb_MessageDef_GetWrapper(aTHX_ mdef);

    // 1. Create a message with an unknown field by parsing
    // We'll use a tag that doesn't exist in TestAllTypesProto2.
    // Tag 999 << 3 | 0 = 7992 (0x1F38)
    // 7992 in varint: 0xB8 0x3E
    // Value 123 (0x7B)
    const char wire_data[] = { 0xB8, 0x3E, 0x7B };
    SV* serialized_sv = newSVpvn(wire_data, sizeof(wire_data));
    
    SV* msg_sv = PerlUpb_Message_Parse(aTHX_ mdef_sv, serialized_sv);
    ok(msg_sv != NULL, "Parsed message with unknown field");

    // 2. Retrieve unknown fields
    SV* set_sv = PerlUpb_UnknownFieldSet_New(aTHX_ msg_sv);
    SV* ret_data = PerlUpb_UnknownFieldSet_GetData(aTHX_ set_sv);
    is(SvCUR(ret_data), sizeof(wire_data), "Retrieved unknown data length matches");
    is_blob(SvPV_nolen(ret_data), wire_data, sizeof(wire_data), "Retrieved unknown data content matches");
    SvREFCNT_dec(ret_data);

    // 3. Serialize and ensure unknown fields are preserved
    SV* reserialized = PerlUpb_Message_Serialize(aTHX_ msg_sv);
    ok(SvCUR(reserialized) >= sizeof(wire_data), "Reserialized length is at least unknown data length");
    
    // Check if wire_data is present in reserialized
    const char* res_ptr = SvPV_nolen(reserialized);
    bool found = false;
    for (size_t i = 0; i <= SvCUR(reserialized) - sizeof(wire_data); i++) {
        if (memcmp(res_ptr + i, wire_data, sizeof(wire_data)) == 0) {
            found = true;
            break;
        }
    }
    ok(found, "Unknown field preserved in reserialization");

    // 4. Clear and verify
    PerlUpb_UnknownFieldSet_Clear(aTHX_ set_sv);
    SV* ret_data2 = PerlUpb_UnknownFieldSet_GetData(aTHX_ set_sv);
    is(SvCUR(ret_data2), 0, "Unknown fields cleared");
    SvREFCNT_dec(ret_data2);

    // Cleanup
    extern void PerlUpb_UnknownFieldSet_Free(pTHX_ SV* sv);
    PerlUpb_UnknownFieldSet_Free(aTHX_ set_sv);
    SvREFCNT_dec(set_sv);
    SvREFCNT_dec(serialized_sv);
    SvREFCNT_dec(reserialized);
    
    PerlUpb_Arena_Destroy(aTHX_ PerlUpb_Message_GetArena(aTHX_ msg_sv));
    PerlUpb_Message_Free(aTHX_ msg_sv);
    SvREFCNT_dec(msg_sv);
    SvREFCNT_dec(mdef_sv);
    PerlUpb_Arena_Destroy(aTHX_ arena_sv);

    test_perl_destroy(my_perl);
    return 0;
}
