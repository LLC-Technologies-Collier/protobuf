use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'text format encoding integration' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    $msg->set_name("integration test");
    
    my $text = $msg->to_text();
    ok($text, 'Encoded text');
    like($text, qr/value: 12345/, 'Contains value');
    
    # Test text formatting doesn't corrupt message or arena
    my $msg2 = test::TestMessage->new();
    $msg2->set_value(999);
    $msg2->to_text();
    
    is($msg2->value, 999, 'Message data intact after text encoding');
};

done_testing();
