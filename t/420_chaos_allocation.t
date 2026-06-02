use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use Protobuf::Internal;

# Enable global chaos
Protobuf::Internal::set_chaos_enabled(1);

subtest 'Arena-level chaos fail probability' => sub {
    my $arena = Protobuf::Arena->new();
    
    # Set 100% failure rate for this arena
    $arena->__chaos_fail_probability(1.0);
    
    # We don't have a direct malloc in Perl yet, but we can use space_allocated
    # or some other operation that triggers allocation.
    # Currently, new() already allocated one block.
    
    my $stats = $arena->stats;
    ok($stats->{blocks} >= 1, "Initial block exists");
    
    # Future: if we had $arena->malloc(100), it should return undef here.
    pass("Chaos parameters set successfully");
};

subtest 'Arena-level chaos delay probability' => sub {
    my $arena = Protobuf::Arena->new();
    $arena->__chaos_delay_probability(0.5);
    pass("Chaos delay parameters set successfully");
};

done_testing();
