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
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

for my $i (1..$NUM_COROS) {
    push @coros, async {
        eval {
            for (1..50) {
                # Force context switching
                cede();

                my $msg = test::TestMessage->new();
                $msg->set_value($i);
                
                cede();
                
                my $sub = test::NestedMessage->new();
                $sub->set_nested_string("coro-$i");
                $msg->set_nested_message($sub);
                
                cede();
                
                my $serialized = $msg->serialize();
                my $msg2 = test::TestMessage->parse($serialized);
                
                die "Value mismatch" unless $msg2->value == $i;
                die "Nested string mismatch" unless $msg2->nested_message->nested_string eq "coro-$i";
            }
        };
        if ($@) {
            push @errors, "Coro $i failed: $@";
        }
    };
}

$_->join for @coros;

is_deeply(\@errors, [], "All coroutines completed without errors");

done_testing();
