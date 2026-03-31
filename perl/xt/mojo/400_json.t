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
                $msg->set_name("mojo test $i $j");
                
                # Defer the JSON decoding to another event loop tick
                Mojo::IOLoop->timer(rand(0.05) => sub {
                    eval {
                        my $json = $msg->to_json();
                        my $msg2 = test::TestMessage->from_json($json);
                        die "JSON roundtrip mismatch for value" unless $msg2->value == ($i * $j);
                        die "JSON roundtrip mismatch for name" unless $msg2->name eq "mojo test $i $j";
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
