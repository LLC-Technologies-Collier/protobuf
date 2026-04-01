use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

# Load descriptors
my $pool = TestHelpers->get_generated_pool();
foreach my $file ('t/data/test_descriptor.bin', 't/data/wkt_descriptor.bin') {
    open my $fh, '<:raw', $file or die "Could not open $file: $!";
    my $data = do { local $/; <$fh> };
    $pool->add_serialized_file_descriptor_set($data);
}

{
    package My::App;
    use Moo;
    use Types::Standard qw( InstanceOf HashRef );
    use Type::Utils qw( coerce );

    # Define a coercion from HashRef to our Message class
    my $TestMsgType = InstanceOf['protobuf_test_messages::proto2::TestAllTypesProto2'];
    my $CoercedTestMsg = $TestMsgType->plus_coercions(
        HashRef, sub {
            my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
            $msg->from_perl($_);
            return $msg;
        }
    );

    has 'message' => (
        is       => 'ro',
        isa      => $CoercedTestMsg,
        coerce   => 1,
    );
}

subtest 'Moo InstanceOf constraint' => sub {
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg->set_optional_int32(42);
    
    my $app = My::App->new(message => $msg);
    ok($app, 'App created with Protobuf object');
    is($app->message->optional_int32, 42, 'Message object preserved');
    isa_ok($app->message, 'protobuf_test_messages::proto2::TestAllTypesProto2');
};

subtest 'Type::Tiny coercion from HashRef' => sub {
    my $app = My::App->new(message => { optional_int32 => 99, optional_string => "coerced" });
    ok($app, 'App created with HashRef (coerced)');
    isa_ok($app->message, 'protobuf_test_messages::proto2::TestAllTypesProto2');
    is($app->message->optional_int32, 99, 'Coerced value correct');
    is($app->message->optional_string, 'coerced', 'Coerced string correct');
};

subtest 'Nested HashRef support' => sub {
    my $app = My::App->new(message => {
        optional_int32 => 1,
        optional_nested_message => {
            a => 123
        }
    });
    
    ok($app, 'App created with nested HashRef');
    is($app->message->optional_nested_message->a, 123, 'Nested data correctly populated');
};

subtest 'to_perl deep conversion' => sub {
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg->set_optional_int32(123);
    $msg->set_optional_string("deep");
    push @{$msg->repeated_int32}, 1, 2, 3;
    $msg->map_int32_int32->{5} = 10;
    
    my $sub = protobuf_test_messages::proto2::TestAllTypesProto2::NestedMessage->new();
    $sub->set_a(999);
    $msg->set_optional_nested_message($sub);
    
    my $data = $msg->to_perl();
    is_deeply($data, {
        optional_int32 => 123,
        optional_string => "deep",
        repeated_int32 => [1, 2, 3],
        map_int32_int32 => { 5 => 10 },
        optional_nested_message => {
            a => 999
        }
    }, 'Deep to_perl HashRef is correct');
};

subtest 'WKT integration with Type::Tiny' => sub {
    # 1. google.protobuf.Struct
    my $struct = google::protobuf::Struct->new();
    $struct->from_perl({ a => 1, b => { c => 3 } });
    
    my $perl = $struct->to_perl();
    is_deeply($perl, { a => 1, b => { c => 3 } }, 'Struct deep conversion matches');
    
    # 2. google.protobuf.Any
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg->set_optional_int32(42);
    
    my $any = google::protobuf::Any->new();
    $any->pack($msg);
    
    my $unpacked = $any->unpack();
    isa_ok($unpacked, 'protobuf_test_messages::proto2::TestAllTypesProto2');
    is($unpacked->optional_int32, 42, 'Any unpacked correctly');
};

done_testing();
