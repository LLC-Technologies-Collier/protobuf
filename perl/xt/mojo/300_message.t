package main;
use strict;
use warnings;
use Test::More;

BEGIN {
    plan skip_all => 'Mojolicious required for Mojo::IOLoop' 
        unless eval { require Mojo::IOLoop; require Mojo::Promise; 1 };
}

use Protobuf::DescriptorPool;
use Protobuf::Arena;

my $pool = Protobuf::DescriptorPool->generated_pool();
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

subtest 'Concurrent message creation and sub-message lifecycle with Mojo::IOLoop' => sub {
    my $concurrency = 100;
    my @promises;
    
    for my $i (1..$concurrency) {
        my $p = Mojo::Promise->new();
        push @promises, $p;
        
        Mojo::IOLoop->timer(rand(0.05) => sub {
            eval {
                # Create a parent message
                my $msg = test::TestMessage->new();
                $msg->set_value($i);
                
                # Defer sub-message creation and parent drop
                Mojo::IOLoop->timer(rand(0.02) => sub {
                    eval {
                        my $sub = test::NestedMessage->new();
                        $sub->set_nested_string("Hello $i");
                        $msg->set_nested_message($sub);
                        
                        # Grab a reference to the repeated array
                        my $arr = $msg->repeated_int;
                        push @$arr, $i, $i+1;
                        
                        # Now intentionally let $msg go out of scope, but keep $arr active
                        # into yet another callback
                        my $val = $msg->value;
                        undef $msg;
                        
                        Mojo::IOLoop->timer(rand(0.02) => sub {
                            eval {
                                # Verify $arr (and thus the underlying UPB Arena) survived!
                                die "Array size mismatch" unless scalar(@$arr) == 2;
                                die "Array data mismatch" unless $arr->[0] == $i;
                                
                                $p->resolve();
                            };
                            if ($@) {
                                $p->reject("Inner timer failed: $@");
                            }
                        });
                    };
                    if ($@) {
                        $p->reject("Middle timer failed: $@");
                    }
                });
            };
            if ($@) {
                $p->reject("Outer timer failed: $@");
            }
        });
    }
    
    my $errors = 0;
    Mojo::Promise->all(@promises)->then(sub {
        pass("All $concurrency complex nested async message operations completed");
    })->catch(sub {
        my ($err) = @_;
        fail("Async operations failed: $err");
        $errors++;
    })->wait;
    
    is($errors, 0, "No ASan crashes or reference lifetime errors detected");
};

done_testing();
