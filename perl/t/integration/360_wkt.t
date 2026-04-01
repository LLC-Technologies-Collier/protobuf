use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
# Load both test and WKT descriptors
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin', 't/data/wkt_descriptor.bin');

subtest 'any packed with other message' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(999);
    
    my $any = google::protobuf::Any->new();
    $any->pack($msg);
    
    my $unpacked = $any->unpack();
    is($unpacked->value, 999, 'Any packs/unpacks TestMessage correctly');
};

subtest 'struct inside listvalue inside struct' => sub {
    my $data = {
        key => [
            { a => 1 },
            { b => "two" }
        ]
    };
    
    my $struct = google::protobuf::Struct->new();
    $struct->from_perl($data);
    
    my $out = $struct->to_perl();
    is_deeply($out, $data, 'Complex nested struct roundtrip works');
    
    my $serialized = $struct->serialize();
    my $parsed = google::protobuf::Struct->parse($serialized);
    is_deeply($parsed->to_perl(), $data, 'Struct serialization roundtrip works');
};

done_testing();
