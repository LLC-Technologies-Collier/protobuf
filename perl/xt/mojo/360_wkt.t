package main;
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
foreach my $file ('t/data/test_descriptor.bin', 't/data/wkt_descriptor.bin') {
    open my $fh, '<:raw', $file or die "Could not open $file: $!";
    my $data = do { local $/; <$fh> };
    $pool->add_serialized_file_descriptor_set($data);
}

for my $i (1..$NUM_TIMERS) {
    # Create some nested closures to ensure variables are captured
    Mojo::IOLoop->timer(rand(0.1) => sub {
        eval {
            for my $j (1..50) {
                my $msg = test::TestMessage->new();
                $msg->set_value($i * $j);

                my $any = google::protobuf::Any->new();
                $any->pack($msg);

                # Defer the unpack to another event loop tick to stress memory management
                Mojo::IOLoop->timer(rand(0.05) => sub {
                    eval {
                        my $unpacked = $any->unpack();
                        die "Unpack failed" unless $unpacked->value == ($i * $j);

                        my $struct = google::protobuf::Struct->new();
                        $struct->from_perl({ a => $i, b => $j });

                        die "Struct mismatch" unless $struct->fields->{a}->number_value == $i;
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
