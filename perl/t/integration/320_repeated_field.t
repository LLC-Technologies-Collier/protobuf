use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

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
