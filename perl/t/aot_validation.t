use strict;
use warnings;
use Test::More;
use Protobuf::ClassGenerator;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
TestHelpers->generate_classes($pool);

subtest 'AOT validator generation' => sub {
    my $mdef = $pool->find_message_by_name('test.TestMessage');
    ok($mdef, "Found TestMessage descriptor");
    
    my $xs = Protobuf::ClassGenerator->generate_validator_xs($mdef);
    diag("Generated XS Validator:\n$xs");
    
    like($xs, qr/bool validate_test_TestMessage\(pTHX_ SV\* sv\)/, "Generated correct function signature");
    like($xs, qr/sv_derived_from\(sv, "test::TestMessage"\)/, "Includes class check");
    
    # Check if it includes required fields (if any in TestMessage)
    # TestMessage has no required fields in proto2 by default unless specified.
    # We can add one to test.proto if needed, but the logic is there.
};

done_testing();
