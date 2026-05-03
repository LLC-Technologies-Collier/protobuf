use strict;
use warnings;
use Test::More;
use Protobuf::Message;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
TestHelpers->generate_classes($pool);

subtest 'intelligent oneof coercion' => sub {
    my $msg = Test::Test::TestMessage->new();
    ok($msg->isa('Protobuf::Message'), "Test::Test::TestMessage inherits from Protobuf::Message");
    
    # 1. Coerce to int branch
    my $branch = $msg->set_oneof('test_oneof', 42);
    is($branch, 'oneof_int32', "Coerced to oneof_int32");
    is($msg->oneof_int32, 42, "Value correctly set");
    is($msg->which_oneof('test_oneof'), 'oneof_int32', "which_oneof agrees");
    
    # 2. Coerce to string branch (overwrites)
    $branch = $msg->set_oneof('test_oneof', "hello");
    is($branch, 'oneof_string', "Coerced to oneof_string");
    is($msg->oneof_string, "hello", "Value correctly set");
    is($msg->which_oneof('test_oneof'), 'oneof_string', "which_oneof agrees");
    
    # 3. Coerce to message branch
    my $sub = Test::Test::NestedMessage->new();
    $sub->nested_string("nested");
    $branch = $msg->set_oneof('test_oneof', $sub);
    is($branch, 'oneof_msg', "Coerced to oneof_msg");
    is($msg->oneof_msg->nested_string, "nested", "Submessage value correct");
};

done_testing();
