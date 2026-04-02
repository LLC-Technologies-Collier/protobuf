package main;
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

my $pool = Protobuf::DescriptorPool->new;
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

for my $i (1..$NUM_COROS) {
    push @coros, async {
        eval {
            for (1..100) {
                # Force context switching
                cede();

                my $message_def = $pool->find_message_by_name('test.TestMessage');
                die "test.TestMessage not found" unless $message_def;

                my $field = $message_def->find_field_by_name('value');
                die "Field 'value' not found" unless $field;

                my $enum_def = $pool->find_enum_by_name('test.TestEnum');
                die "Enum 'test.TestEnum' not found" unless $enum_def;
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
    local $TODO = 'Stress concurrent DescriptorPool lookups from 100 coroutines';
    ok(0, 'High-frequency concurrent pool access is stable and lock-free');
}

TODO: {
    local $TODO = 'Verify Coro re-entrancy during descriptor additions';
    ok(0, 'System handles coroutine context switches during descriptor set loading safely');
}

done_testing();
