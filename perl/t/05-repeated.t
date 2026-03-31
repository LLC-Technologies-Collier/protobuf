use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

subtest 'repeated scalar field operations' => sub {
    my $msg = test::TestMessage->new();
    my $arr = $msg->repeated_int;
    
    ok($arr, 'Got repeated int array');
    is(scalar(@$arr), 0, 'Initial size is 0');
    
    push @$arr, 10, 20, 30;
    is_deeply($arr, [10, 20, 30], 'Push works');
    
    my $val = pop @$arr;
    is($val, 30, 'Pop returns correct value');
    is(scalar(@$arr), 2, 'Size after pop is 2');
    
    unshift @$arr, 5;
    is_deeply($arr, [5, 10, 20], 'Unshift works');
    
    my $shifted = shift @$arr;
    is($shifted, 5, 'Shift returns correct value');
    is_deeply($arr, [10, 20], 'Size after shift is 2');
    
    $arr->[0] = 100;
    is($arr->[0], 100, 'Index assignment works');
    
    # Store size
    $#$arr = 4;
    is(scalar(@$arr), 5, 'Setting $#arr resizes array');
    # UPB initializes new elements to 0
    is($arr->[4], 0, 'New elements are zero-initialized');
    
    # Clear
    @$arr = ();
    is(scalar(@$arr), 0, 'Clearing array works');
};

subtest 'repeated message field operations' => sub {
    my $msg = test::TestMessage->new();
    my $arr = $msg->repeated_message;
    
    my $sub1 = test::NestedMessage->new();
    $sub1->set_nested_string("m1");
    
    my $sub2 = test::NestedMessage->new();
    $sub2->set_nested_string("m2");
    
    push @$arr, $sub1, $sub2;
    is(scalar(@$arr), 2, 'Push messages works');
    is($arr->[0]->nested_string, "m1", 'Index 0 message data correct');
    
    unshift @$arr, test::NestedMessage->new();
    $arr->[0]->set_nested_string("m0");
    is($arr->[0]->nested_string, "m0", 'Unshift message works');
    is($arr->[1]->nested_string, "m1", 'Old index 0 moved to index 1');
    
    my $shifted = shift @$arr;
    is($shifted->nested_string, "m0", 'Shift message works');
    is($arr->[0]->nested_string, "m1", 'New index 0 correct');
};

done_testing();
