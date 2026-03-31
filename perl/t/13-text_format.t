use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'text format encoding' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    $msg->set_name("hello world");
    
    my $sub = test::NestedMessage->new();
    $sub->set_nested_string("inner");
    $msg->set_nested_message($sub);
    
    # We expect some methods on Protobuf::Message
    my $text = $msg->to_text();
    ok(defined $text, 'Text format is generated');
    like($text, qr/value: 12345/, 'Contains integer field');
    like($text, qr/name: "hello world"/, 'Contains string field');
    like($text, qr/nested_message \{.*nested_string: "inner".*\}/s, 'Contains nested message');
};

done_testing();
