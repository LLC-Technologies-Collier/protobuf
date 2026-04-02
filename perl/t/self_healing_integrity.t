use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
TestHelpers->generate_classes($pool);

subtest 'self-healing integrity audit' => sub {
    my $msg = test::TestMessage->new();
    
    # 1. Healthy state
    my $sub = test::NestedMessage->new();
    $sub->nested_string("hello");
    $msg->nested_message($sub);
    
    ok($msg->audit_integrity(), "Message passes integrity audit (healthy)");
    
    # 2. Simulate corruption: replace reified object in hash with a sibling
    # Access the reified object to ensure it's in the cache
    my $reified = $msg->nested_message;
    ok($reified, "Got reified submessage");
    
    # Manually tamper with the Perl hash (simulating bad user code or bug)
    my $fake = test::NestedMessage->new();
    $msg->{nested_message} = $fake;
    
    ok(!$msg->audit_integrity(), "Audit correctly detects identity mismatch (corruption)");
    
    # 3. Clean up and verify
    $msg->nested_message($sub);
    ok($msg->audit_integrity(), "Audit passes after fixing corruption");
};

done_testing();
