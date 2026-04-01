use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'json integration' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(123);
    $msg->set_name("integration");
    
    my $json = $msg->to_json();
    
    my $msg2 = test::TestMessage->from_json($json);
    
    is($msg2->value, 123, 'Value matches after json roundtrip');
    is($msg2->name, 'integration', 'Name matches after json roundtrip');
};

done_testing();
