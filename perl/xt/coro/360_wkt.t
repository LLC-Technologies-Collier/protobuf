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
foreach my $file ('t/data/test_descriptor.bin', 't/data/wkt_descriptor.bin') {
    open my $fh, '<:raw', $file or die "Could not open $file: $!";
    my $data = do { local $/; <$fh> };
    $pool->add_serialized_file_descriptor_set($data);
}

for my $i (1..$NUM_COROS) {
    push @coros, async {
        eval {
            for my $j (1..50) {
                cede();

                my $msg = test::TestMessage->new();
                $msg->set_value($i * $j);
                
                cede();
                
                my $any = google::protobuf::Any->new();
                $any->pack($msg);
                
                cede();
                
                my $unpacked = $any->unpack();
                die "Unpack failed" unless $unpacked->value == ($i * $j);
                
                cede();
                
                my $struct = google::protobuf::Struct->new();
                $struct->from_perl({ a => $i, b => $j });
                
                die "Struct mismatch" unless $struct->fields->{a}->number_value == $i;
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
