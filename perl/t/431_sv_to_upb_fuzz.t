use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'Type-Fuzzing: SV to Upb' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    # 1. CodeRef
    eval { $msg->set_value(sub { 123 }) };
    ok($@, "Setting field to CodeRef croaks");
    like($@, qr/did not pass type constraint "Int"/, "Correct error for CodeRef");

    # 2. Glob
    eval { $msg->set_value(*STDIN) };
    ok($@, "Setting field to Glob croaks");
    like($@, qr/did not pass type constraint "Int"/, "Correct error for Glob");

    # 3. HashRef for scalar field
    eval { $msg->set_value({ a => 1 }) };
    ok($@, "Setting field to HashRef croaks");
    like($@, qr/did not pass type constraint "Int"/, "Correct error for HashRef");
};

subtest 'Range-checks for narrow integers' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    # Int32 overflow
    eval { $msg->set_value(2**31) };
    ok($@, "Int32 overflow croaks");
    like($@, qr/out of range/, "Correct error for Int32 overflow");

    # Int32 underflow
    eval { $msg->set_value(-2**31 - 1) };
    ok($@, "Int32 underflow croaks");
    like($@, qr/out of range/, "Correct error for Int32 underflow");

    # UInt32 overflow
    eval { $msg->set_optional_uint32(2**32) };
    ok($@, "UInt32 overflow croaks");
    like($@, qr/out of range/, "Correct error for UInt32 overflow");

    # UInt32 negative
    eval { $msg->set_optional_uint32(-1) };
    ok($@, "UInt32 negative croaks");
    like($@, qr/out of range/, "Correct error for UInt32 negative");
};

done_testing();
