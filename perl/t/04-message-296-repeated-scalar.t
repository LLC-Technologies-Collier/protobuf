use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use lib "t/lib";
use TestHelpers;

# Load descriptors into the generated pool
my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'repeated scalar field accessors' => sub {
    my $msg = test::TestMessage->new();
    
    ok($msg->can('repeated_int'), 'Generated getter for repeated_int');
    my $arr = $msg->repeated_int;
    ok($arr, 'Got repeated field object');
    isa_ok($arr, 'ARRAY', 'It is an array reference (tied)');
    is(scalar(@$arr), 0, 'Initial size is 0');
    
    # Push elements
    push @$arr, 10, 20, 30;
    is(scalar(@$arr), 3, 'Size after push is 3');
    is($arr->[0], 10, 'Index 0 is correct');
    is($arr->[1], 20, 'Index 1 is correct');
    is($arr->[2], 30, 'Index 2 is correct');
    
    # Pop
    my $last = pop @$arr;
    is($last, 30, 'Popped correct value');
    is(scalar(@$arr), 2, 'Size after pop is 2');
    
    # Assignment by index
    $arr->[0] = 100;
    is($arr->[0], 100, 'Assigned index correctly');
    
    # Verify via base get method (returns un-tied array ref currently? No, it should return the same tied one)
    my $arr2 = $msg->get('repeated_int');
    is_deeply($arr2, [100, 20], 'get() returns expected values');
};

done_testing();
