use strict;
use warnings;
use Test::More;
use Protobuf;
use lib "t/lib";
use TestHelpers;
use Scalar::Util qw(refaddr);

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/compat_descriptor.bin', 't/data/test_descriptor.bin');

subtest 'Stable Object Identity' => sub {
    my $msg = Protobuf::Types::Types->new();
    
    # 1. Message Identity
    my $sub = Protobuf::Types::Types::Message->new(t_string => 'stable');
    $msg->set_opt_message($sub);
    
    my $got1 = $msg->opt_message;
    my $got2 = $msg->opt_message;
    
    ok($got1, 'Got sub-message');
    is(refaddr($got1), refaddr($got2), 'Repeated access to same sub-message returns identical SV');

    # 2. Repeated Container Identity
    my $rep1 = $msg->rep_int32;
    my $rep2 = $msg->rep_int32;
    
    ok($rep1, 'Got repeated field');
    is(refaddr($rep1), refaddr($rep2), 'Repeated access to same repeated field returns identical SV');

    # 3. Map Container Identity
    my $msg2 = Protobuf_test_messages::Proto2::TestMessagesProto2::TestAllTypesProto2->new();
    my $map1 = $msg2->map_int32_int32;
    my $map2 = $msg2->map_int32_int32;
    
    ok($map1, 'Got map field');
    is(refaddr($map1), refaddr($map2), 'Repeated access to same map field returns identical SV');
};

done_testing();
