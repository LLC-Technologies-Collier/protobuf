use strict;
use warnings;
use Test::More;
use Protobuf;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/compat_descriptor.bin');

subtest 'int32 boundary checks' => sub {
    my $msg = Protobuf::Types::Types->new();
    
    # 1. Valid range
    eval { $msg->set_opt_int32(2147483647) };
    ok(!$@, 'Set INT32_MAX success') or diag($@);
    is($msg->opt_int32, 2147483647, 'Value matches');
    
    eval { $msg->set_opt_int32(-2147483648) };
    ok(!$@, 'Set INT32_MIN success') or diag($@);
    is($msg->opt_int32, -2147483648, 'Value matches');
    
    # 2. Positive Overflow
    eval { $msg->set_opt_int32(2147483648) };
    like($@, qr/out of range/i, 'Set INT32_MAX + 1 failed');
    
    # 3. Negative Overflow
    eval { $msg->set_opt_int32(-2147483649) };
    like($@, qr/out of range/i, 'Set INT32_MIN - 1 failed');
};

subtest 'uint32 boundary checks' => sub {
    my $msg = Protobuf::Types::Types->new();
    
    # 1. Valid range
    eval { $msg->set_opt_uint32(4294967295) };
    ok(!$@, 'Set UINT32_MAX success') or diag($@);
    is($msg->opt_uint32, 4294967295, 'Value matches');
    
    # 2. Negative
    eval { $msg->set_opt_uint32(-1) };
    like($@, qr/out of range/i, 'Set negative to uint32 failed');
    
    # 3. Overflow
    eval { $msg->set_opt_uint32(4294967296) };
    like($@, qr/out of range/i, 'Set UINT32_MAX + 1 failed');
};

done_testing();
