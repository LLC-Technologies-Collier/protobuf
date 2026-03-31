use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

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
