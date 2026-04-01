use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
# Load both test and WKT descriptors
foreach my $file ('t/data/test_descriptor.bin', 't/data/wkt_descriptor.bin') {
    open my $fh, '<:raw', $file or die "Could not open $file: $!";
    my $data = do { local $/; <$fh> };
    $pool->add_serialized_file_descriptor_set($data);
}

subtest 'any packing and unpacking' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    
    my $any = google::protobuf::Any->new();
    $any->pack($msg);
    
    is($any->type_url, "type.googleapis.com/test.TestMessage", 'Any type_url correct');
    ok(length($any->value) > 0, 'Any value contains data');
    
    # Unpack
    my $unpacked = $any->unpack();
    isa_ok($unpacked, 'test::TestMessage');
    is($unpacked->value, 12345, 'Unpacked data matches original');
};

done_testing();
