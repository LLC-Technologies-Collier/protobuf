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

subtest 'repeated field cross-message interaction' => sub {
    my $msg1 = test::TestMessage->new();
    push @{$msg1->repeated_int}, 10, 20, 30;
    
    diag("msg1->repeated_int type: " . ref($msg1->repeated_int));
    
    my $msg2 = test::TestMessage->new();
    # This should copy elements
    $msg2->set('repeated_int', $msg1->repeated_int);
    
    is_deeply($msg2->repeated_int, [10, 20, 30], 'Repeated field copied to another message');
    
    # Verify they are independent
    push @{$msg1->repeated_int}, 40;
    is(scalar(@{$msg2->repeated_int}), 3, 'Target message independent of source after copy');
};

subtest 'repeated message cross-message interaction' => sub {
    my $msg1 = test::TestMessage->new();
    my $sub = test::NestedMessage->new();
    $sub->set_nested_string("orig");
    push @{$msg1->repeated_message}, $sub;
    
    my $msg2 = test::TestMessage->new();
    $msg2->set('repeated_message', $msg1->repeated_message);
    
    is($msg2->repeated_message->[0]->nested_string, "orig", 'Repeated messages copied');
    
    # Verify independent
    $msg1->repeated_message->[0]->set_nested_string("changed");
    is($msg2->repeated_message->[0]->nested_string, "orig", 'Messages in target are independent (copied)');
};

done_testing();
