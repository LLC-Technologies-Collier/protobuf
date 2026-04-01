#include "t/c/upb-perl-test.h"
#include "xs/protobuf.h"
#include "xs/extension_dict/dict.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/message.h"

int main(int argc, char** argv) {
    PerlInterpreter *my_perl = test_perl_init(argc, argv);

    plan(3);

    // Mock MessageDef and FieldDef (Extension)
    // In a real test, we'd load these from a pool.
    SV* msg_sv = newSViv(1);
    sv_bless(newRV_noinc(msg_sv), gv_stashpv("Protobuf::Descriptor::MessageDef", GV_ADD));
    
    // 1. Creation
    SV* dict_sv = PerlUpb_ExtensionDict_New(aTHX_ msg_sv);
    ok(dict_sv != NULL, "PerlUpb_ExtensionDict_New returns non-NULL");
    ok(sv_derived_from(dict_sv, "Protobuf::Internal::ExtensionDict"), "Dict SV has correct class");

    // 2. Retrieval of message
    SV* msg_back = PerlUpb_ExtensionDict_GetMessageSV(aTHX_ dict_sv);
    is(SvRV(msg_back), msg_sv, "ExtensionDict returns correct parent message SV");

    // Cleanup
    SvREFCNT_dec(dict_sv);
    SvREFCNT_dec(msg_sv);

    test_perl_destroy(my_perl);
    return 0;
}
