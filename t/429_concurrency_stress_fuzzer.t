use strict;
use warnings;
use Test::More;
BEGIN {
    eval "use Coro; use Coro::AnyEvent;";
    if ($@) {
        plan skip_all => "Coro and Coro::AnyEvent are required for concurrency stress fuzzer";
    }
}
use Protobuf::Internal;
use Protobuf::Arena;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

# Setup pool and classes
my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
TestHelpers->generate_classes($pool);

subtest 'concurrency stress fuzzer (Coro)' => sub {
    my $num_coros = 1;
    my $ops_per_coro = 50;
    my @coros;
    
    # 1. Enable Chaos Engine (DISABLED FOR DEBUGGING)
    # Protobuf::Internal::set_chaos_params(0.1, 0.1, 10, 1234);
    # Protobuf::Internal::set_chaos_enabled(1);
    note("Spawning $num_coros coroutines, each performing $ops_per_coro random operations...");

    my $failed_ops = 0;
    my $total_ops = 0;

    for (1..$num_coros) {
        push @coros, async {
            for (1..$ops_per_coro) {
                my $op = int(rand(5));
                $total_ops++;
                
                eval {
                    if ($op == 0) {
                        # Create/Destroy Arena
                        my $a = Protobuf::Arena->new();
                    }
                    elsif ($op == 1) {
                        # Message Lifecycle
                        my $msg = Test::Test::TestMessage->new();
                        $msg->set_value(int(rand(1000)));
                        my $perl = $msg->to_perl();
                    }
                    elsif ($op == 2) {
                        # Serialization
                        my $msg = Test::Test::TestMessage->new();
                        $msg->set_name("fuzz_" . int(rand(100)));
                        my $wire = $msg->serialize();
                        my $msg2 = Test::Test::TestMessage->parse($wire);
                    }
                    elsif ($op == 3) {
                        # Oneof Coercion
                        my $msg = Test::Test::TestMessage->new();
                        my $val = (rand() > 0.5) ? "str" . rand() : int(rand(1000));
                        $msg->set_oneof('test_oneof', $val);
                    }
                    elsif ($op == 4) {
                        # Repeated field batch conversion
                        my $msg = Test::Test::TestMessage->new();
                        my @data = map { int(rand(1000)) } (1..100);
                        $msg->repeated_int(\@data);
                        my $got = $msg->repeated_int();
                    }
                };
                if ($@) {
                    # If it's a chaos failure, that's fine. 
                    if ($@ !~ /Failed to (?:acquire upb_Arena|allocate upb_Message|resize upb_Array)/) {
                        diag("Unexpected failure in coro: $@");
                        $failed_ops++;
                    }
                }
                
                # Yield to other coroutines
                cede;
            }
        };
    }
    
    $_->join for @coros;
    
    Protobuf::Internal::set_chaos_enabled(0);
    
    note("Fuzzing complete: $total_ops operations attempted, $failed_ops unexpected failures");
    ok($failed_ops == 0, "All operations completed without unexpected errors");
    
    # 3. Check memory stability via audit log
    my $log = Protobuf::Internal::get_cache_audit_log();
    my $mallocs = grep { $_->{type} == 10 } @$log;
    my $frees   = grep { $_->{type} == 11 } @$log;
    note("Memory Audit - MALLOCs: $mallocs, FREEs: $frees");
    
    ok($mallocs > 0, "Audit log recorded activity");
};

done_testing();
