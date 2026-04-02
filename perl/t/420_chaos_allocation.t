use strict;
use warnings;
use Test::More;
use Protobuf::Internal;
use Protobuf::Arena;

subtest 'chaos engine: forced failure' => sub {
    # 1. Enable chaos with 100% failure probability
    Protobuf::Internal::set_chaos_params(1.0, 0.0, 0, 42);
    Protobuf::Internal::set_chaos_enabled(1);
    
    # 2. Attempt to create arena (which uses stats/chaos allocator internally)
    # Actually, PerlUpb_Arena_New calls safemalloc for the wrapper, 
    # then PerlUpb_Arena_AcquireWithStats for the upb_Arena.
    # The upb_Arena_Init will call our chaos allocator.
    
    eval {
        my $arena = Protobuf::Arena->new();
    };
    ok($@, "Arena creation failed under 100% chaos failure probability");
    like($@, qr/Failed to acquire upb_Arena/, "Correct error message from forced failure");
    
    # 3. Disable chaos
    Protobuf::Internal::set_chaos_enabled(0);
    
    # 4. Attempt to create arena again
    my $arena2;
    eval {
        $arena2 = Protobuf::Arena->new();
    };
    ok($arena2, "Arena creation succeeded after disabling chaos");
    isa_ok($arena2, 'Protobuf::Arena');
};

subtest 'chaos engine: probability' => sub {
    # Set to 50% failure
    Protobuf::Internal::set_chaos_params(0.5, 0.0, 0, 123);
    Protobuf::Internal::set_chaos_enabled(1);
    
    my $fails = 0;
    my $successes = 0;
    
    for (1..20) {
        eval {
            my $a = Protobuf::Arena->new();
        };
        if ($@) { $fails++; } else { $successes++; }
    }
    
    diag("Chaos Results (50% failure): $successes successes, $fails failures");
    ok($fails > 0, "At least one failure recorded");
    ok($successes > 0, "At least one success recorded");
    
    Protobuf::Internal::set_chaos_enabled(0);
};

subtest 'chaos engine: delay' => sub {
    use Time::HiRes qw(gettimeofday tv_interval);
    
    # Set to 100% delay of 100ms
    Protobuf::Internal::set_chaos_params(0.0, 1.0, 100, 789);
    Protobuf::Internal::set_chaos_enabled(1);
    
    my $t0 = [gettimeofday];
    my $arena = Protobuf::Arena->new();
    my $elapsed = tv_interval($t0);
    
    diag("Chaos Delay: ${elapsed}s");
    ok($elapsed > 0.05, "Observed significant delay in allocation");
    
    Protobuf::Internal::set_chaos_enabled(0);
};

done_testing();
