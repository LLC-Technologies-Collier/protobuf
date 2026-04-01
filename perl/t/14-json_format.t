use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'json format encoding' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    $msg->set_name("hello json");
    
    my $sub = test::NestedMessage->new();
    $sub->set_nested_string("inner_json");
    $msg->set_nested_message($sub);
    
    my $json = $msg->to_json();
    ok(defined $json, 'JSON format is generated');
    like($json, qr/"value":12345/, 'Contains integer field');
    like($json, qr/"name":"hello json"/, 'Contains string field');
    like($json, qr/"nestedMessage":\{"nestedString":"inner_json"\}/, 'Contains nested message');
};

subtest 'json format decoding' => sub {
    my $json = '{"value":999,"name":"parsed","nestedMessage":{"nestedString":"nested_parsed"}}';
    my $msg = test::TestMessage->from_json($json);
    
    ok(defined $msg, 'Parsed from JSON');
    is($msg->value, 999, 'Parsed value correctly');
    is($msg->name, 'parsed', 'Parsed name correctly');
    is($msg->nested_message->nested_string, 'nested_parsed', 'Parsed nested message correctly');
};

done_testing();
