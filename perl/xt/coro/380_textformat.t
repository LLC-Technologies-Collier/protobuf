use strict;
use warnings;
use Test::More;

BEGIN {
    plan skip_all => "Coro is required for concurrent testing" unless eval { require Coro; 1 };
}
use Coro;
use Protobuf::Arena;
use Protobuf::DescriptorPool;

my $NUM_COROS = 10;
my @coros;
my @errors;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

for my $i (1..$NUM_COROS) {
    push @coros, async {
        eval {
            for my $j (1..50) {
                cede();

                my $msg = test::TestMessage->new();
                $msg->set_value($i * $j);
                
                cede();
                
                my $text = $msg->to_text();
                
                cede();
                
                die "Text mismatch" unless $text =~ /value: ${\($i * $j)}/;
            }
        };
        if ($@) {
            push @errors, "Coro $i failed: $@";
        }
    };
}

$_->join for @coros;

is_deeply(\@errors, [], "All coroutines completed without errors");

TODO: {
    local $TODO = 'Stress concurrent TextFormat parsing from 100 coroutines';
    ok(0, 'System remains stable under massive concurrent text parsing');
}

TODO: {
    local $TODO = 'Verify Coro re-entrancy during text formatting of complex trees';
    ok(0, 'Context switches during large text generation do not cause corruption');
}

done_testing();
