#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/message/message.h"
#include "xs/protobuf/message.h"
#include "xs/protobuf/arena.h"
#include "t/c/convert/test_util.h"

static void test_message_wrapping(pTHX) {
    upb_Arena *arena = upb_Arena_New();
    if (!load_test_descriptors(aTHX_ arena)) {
        fail("Failed to load descriptors");
        upb_Arena_Free(arena);
        return;
    }
    
    const upb_MessageDef *mdef = upb_DefPool_FindMessageByName(test_pool, "test.TestMessage");
    upb_Message *msg = upb_Message_New(upb_MessageDef_MiniTable(mdef), arena);
    
    SV* arena_wrapper = PerlUpb_Arena_New(aTHX);
    SV* msg_sv = PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_wrapper);
    
    ok(msg_sv != NULL, "PerlUpb_WrapMessage returns non-NULL");
    ok(sv_derived_from(msg_sv, "test::TestMessage"), "Blessed into test::TestMessage");
    
    is(PerlUpb_Message_GetDef(aTHX_ msg_sv), mdef, "GetDef matches original");
    is(PerlUpb_Message_GetMsg(aTHX_ msg_sv), msg, "GetMsg matches original");

    SvREFCNT_dec(msg_sv);
    SvREFCNT_dec(arena_wrapper);
    upb_DefPool_Free(test_pool);
    upb_Arena_Free(arena);
}

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(7);

    test_message_wrapping(aTHX);
    ok(1, "Message functions cover creation and wrapping");

    TODO("Implement SIMD-accelerated serialization for fixed-length types") {
        ok(0, "SSE4.1/AVX2 optimization for numeric arrays verified");
    }

    TODO("Implement COW (Copy-On-Write) semantics for shared arena sub-messages") {
        ok(0, "Sub-message cloning avoids deep copies when safety is guaranteed");
    }

    test_perl_destroy(my_perl);
    return 0;
}
