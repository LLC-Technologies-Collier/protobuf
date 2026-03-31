use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

subtest 'map with scalar keys and values' => sub {
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    my $map = $msg->map_int32_int32;
    
    $map->{1} = 100;
    $map->{2} = 200;
    
    is(scalar(keys %$map), 2, 'Size is 2');
    is($map->{1}, 100, 'Key 1 correct');
    is($map->{2}, 200, 'Key 2 correct');
    
    ok(exists $map->{1}, 'Exists 1');
    ok(!exists $map->{3}, 'Not exists 3');
    
    delete $map->{1};
    is(scalar(keys %$map), 1, 'Size after delete');
    ok(!exists $map->{1}, 'Key 1 gone');
    
    %$map = ();
    is(scalar(keys %$map), 0, 'Cleared');
};

subtest 'map with string keys and message values' => sub {
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    my $map = $msg->map_string_nested_message;
    
    my $sub = protobuf_test_messages::proto2::TestAllTypesProto2::NestedMessage->new();
    $sub->set_a(42);
    
    $map->{key} = $sub;
    
    ok(exists $map->{key}, 'Message stored in map');
    my $got = $map->{key};
    isa_ok($got, 'protobuf_test_messages::proto2::TestAllTypesProto2::NestedMessage');
    is($got->a, 42, 'Message data correct');
    
    # Iteration
    my @keys = keys %$map;
    is($keys[0], 'key', 'Iteration keys correct');
    
    my @values = values %$map;
    is($values[0]->a, 42, 'Iteration values correct');
};

subtest 'map type conversion and validation' => sub {
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    my $map = $msg->map_int32_int32;
    
    # Test string to int conversion for keys (common in JSON/CLI)
    $map->{"123"} = "456";
    is($map->{123}, 456, 'Numeric strings converted to integers');
    
    eval { $map->{not_a_number} = 1 };
    ok($@, 'Invalid key type throws error');
    
    eval { $map->{1} = "not_a_number" };
    ok($@, 'Invalid value type throws error');
};

done_testing();
