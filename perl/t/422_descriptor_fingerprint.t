use strict;
use warnings;
use Test::More;
use Protobuf;
plan skip_all => 'XS required for descriptor fingerprinting tests' unless $Protobuf::HAS_XS;
use Protobuf::Internal;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

subtest 'descriptor fingerprinting' => sub {
    my $pool = TestHelpers->get_generated_pool();
    TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    
    my $mdef = $pool->find_message_by_name('test.TestMessage');
    ok($mdef, "Found test.TestMessage");
    
    # 1. Get fingerprint
    my $fingerprint = Protobuf::Internal::get_fingerprint($mdef);
    ok($fingerprint > 0, "Got non-zero fingerprint: $fingerprint");
    
    # 2. Register fingerprint
    Protobuf::Internal::register_fingerprint($mdef);
    
    # 3. Lookup by fingerprint
    my $got = Protobuf::Internal::find_by_fingerprint($fingerprint);
    ok($got, "Found descriptor by fingerprint");
    is($got->full_name, 'test.TestMessage', "Correct message found");
    
    # 4. Verify O(1) stability
    my $got2 = Protobuf::Internal::find_by_fingerprint($fingerprint);
    is($got2, $got, "Repeated lookup returns same object (cached)");
    
    # 5. Non-existent fingerprint
    my $missing = Protobuf::Internal::find_by_fingerprint(123456789);
    ok(!defined($missing), "Correctly returns undef for unknown fingerprint");
};

done_testing();
