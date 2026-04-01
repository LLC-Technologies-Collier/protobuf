use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'map cross-message copy' => sub {
    my $msg1 = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg1->map_string_string->{a} = "A";
    $msg1->map_string_string->{b} = "B";
    
    my $msg2 = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg2->set('map_string_string', $msg1->map_string_string);
    
    is(scalar(keys %{$msg2->map_string_string}), 2, 'Map copied to another message');
    is($msg2->map_string_string->{a}, "A", 'Value A preserved');
    
    # Verify independence
    $msg1->map_string_string->{a} = "CHANGED";
    is($msg2->map_string_string->{a}, "A", 'Target map independent of source after copy');
};

subtest 'message map cross-message copy' => sub {
    my $msg1 = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    my $sub = protobuf_test_messages::proto2::TestAllTypesProto2::NestedMessage->new();
    $sub->set_a(100);
    $msg1->map_string_nested_message->{key} = $sub;
    
    my $msg2 = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg2->set('map_string_nested_message', $msg1->map_string_nested_message);
    
    is($msg2->map_string_nested_message->{key}->a, 100, 'Message map copied');
    
    # Verify independence
    $msg1->map_string_nested_message->{key}->set_a(200);
    is($msg2->map_string_nested_message->{key}->a, 100, 'Messages in target map are independent (deep copied)');
};

done_testing();
