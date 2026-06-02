use strict;
use warnings;
use Test::More;
use Protobuf;
plan skip_all => 'XS required for zero-copy coercion tests' unless $Protobuf::HAS_XS;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
TestHelpers->generate_classes($pool);

# Manually simulate a sibling class for coercion
{
    package test::CoercedMessage;
    use Moo;
    extends 'Test::Test::TestMessage';
}

subtest 'zero-copy type coercion' => sub {
    my $msg = Test::Test::TestMessage->new();
    $msg->set_value(42);
    
    # 1. Coerce to sibling
    my $coerced = $msg->coerce_to('test::CoercedMessage');
    ok($coerced, "Coerced to test::CoercedMessage");
    isa_ok($coerced, 'test::CoercedMessage');
    
    # 2. Verify shared memory (Zero-Copy)
    is($coerced->value(), 42, "Coerced object sees original value");
    
    $msg->set_value(100);
    is($coerced->value(), 100, "Coerced object sees change in original (shared memory)");
    
    $coerced->set_value(200);
    is($msg->value(), 200, "Original object sees change in coerced (shared memory)");
};

done_testing();
