use strict;
use warnings;
use Test::More;

BEGIN {
    plan skip_all => "Mojo::IOLoop is required for concurrent testing" unless eval { require Mojo::IOLoop; 1 };
}
use Mojo::IOLoop;
use Protobuf::Arena;
use Protobuf::DescriptorPool;

my $NUM_TIMERS = 10;
my @errors;
my $completed = 0;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

for my $i (1..$NUM_TIMERS) {
    Mojo::IOLoop->timer(rand(0.1) => sub {
        eval {
            for my $j (1..50) {
                my $msg = test::TestMessage->new();
                $msg->set_value($i * $j);
                
                # Defer the text encoding to another event loop tick to stress memory management
                Mojo::IOLoop->timer(rand(0.05) => sub {
                    eval {
                        my $text = $msg->to_text();
                        die "Text mismatch" unless $text =~ /value: ${\($i * $j)}/;
                    };
                    if ($@) {
                        push @errors, "Timer $i-$j delayed failed: $@";
                    }
                    
                    $completed++;
                    if ($completed == ($NUM_TIMERS * 50)) {
                        Mojo::IOLoop->stop;
                    }
                });
            }
        };
        if ($@) {
            push @errors, "Timer $i failed: $@";
            $completed += 50; # Assume they all failed so we don't hang
            if ($completed >= ($NUM_TIMERS * 50)) {
                Mojo::IOLoop->stop;
            }
        }
    });
}

Mojo::IOLoop->start;

is_deeply(\@errors, [], "All Mojo timers completed without errors");

done_testing();
