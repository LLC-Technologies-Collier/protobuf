use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
# Load descriptors
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin', 't/data/wkt_descriptor.bin');

subtest 'Final Integration: Everything together' => sub {
    # 1. Create a complex message
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg->set_optional_int32(42);
    $msg->set_optional_string("Ultimate Answer");

    # 2. Add repeated elements
    push @{$msg->repeated_int32}, 10, 20, 30;

    # 3. Add map elements
    $msg->map_int32_int32->{100} = 1000;
    $msg->map_int32_int32->{200} = 2000;

    # 4. Use WKT (Struct) inside Any, and pack it
    my $struct = google::protobuf::Struct->new();
    $struct->from_perl({ answer => 42, question => "unknown" });

    my $any = google::protobuf::Any->new();
    $any->pack($struct);

    # Unpack it to prove it works
    my $unpacked_struct = $any->unpack();
    is($unpacked_struct->fields->{'answer'}->number_value, 42, 'WKT unpacked successfully');

    # 5. Serialize and Deserialize (Wire)
    my $wire = $msg->serialize();
    my $parsed = protobuf_test_messages::proto2::TestAllTypesProto2->parse($wire);
    
    is($parsed->optional_int32, 42, 'Wire format parsed value matches');
    is_deeply($parsed->repeated_int32, [10, 20, 30], 'Wire format parsed array matches');
    is($parsed->map_int32_int32->{200}, 2000, 'Wire format parsed map matches');

    # 6. JSON format
    my $json = $msg->to_json();
    like($json, qr/"Ultimate Answer"/, 'JSON format includes name');
    
    my $parsed_json = protobuf_test_messages::proto2::TestAllTypesProto2->from_json($json);
    is($parsed_json->optional_int32, 42, 'JSON format parsed value matches');
    is_deeply($parsed_json->repeated_int32, [10, 20, 30], 'JSON format parsed array matches');
    
    # 7. Text Format
    my $text = $msg->to_text();
    like($text, qr/Ultimate Answer/, 'Text format includes name');
};

done_testing();
