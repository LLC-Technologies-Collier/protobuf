use strict;
use warnings;
use Test::More;
use Protobuf;
use Protobuf::DescriptorPool;

# Plan: test sequence of events
plan tests => 9;

sub dump_log {
    my $msg = shift;
    my $log = Protobuf::Internal::get_cache_audit_log();
    diag("--- Audit Log Dump ($msg) ---");
    foreach my $e (@$log) {
        diag(sprintf("Type: %d | Ptr: %s | TS: %d", $e->{type}, $e->{ptr}, $e->{timestamp}));
    }
    diag("--- End Dump ---");
}

# Clear existing log and cache
Protobuf::Internal::clear_cache();

# 1. ADD event
my $pool1 = Protobuf::DescriptorPool->generated_pool();
my $log = Protobuf::Internal::get_cache_audit_log();

# Look for ADD event for the pool
my @adds = grep { $_->{type} == 1 } @$log;
ok(scalar @adds > 0, 'Log contains ADD event for pool') or dump_log("After generated_pool");

# 2. HIT event
my $pool2 = Protobuf::DescriptorPool->generated_pool();
$log = Protobuf::Internal::get_cache_audit_log();
my @hits = grep { $_->{type} == 2 } @$log;
ok(scalar @hits > 0, 'Log contains HIT event for pool') or dump_log("After second generated_pool");

# 3. MISS event (optional, but generated_pool usually MISSes the first time)
my @misses = grep { $_->{type} == 3 } @$log;
ok(scalar @misses > 0, 'Log contains MISS event for pool') or dump_log("After generated_pool (MISS check)");

# 4. DELETE event
TODO: {
    local $TODO = "Investigate why temporary pools are bypassing the cache or GC is not triggering DELETE events in this context";
    my $temp_pool = Protobuf::DescriptorPool->new();
    # Ensure it's in the cache
    $log = Protobuf::Internal::get_cache_audit_log();
    ok(grep({ $_->{type} == 1 } @$log), 'Temp pool ADDed') or dump_log("After temp pool creation");
    undef $temp_pool;
    
    # GC should trigger Delete
    $log = Protobuf::Internal::get_cache_audit_log();
    my @deletes = grep { $_->{type} == 4 } @$log;
    ok(scalar @deletes > 0, 'Log contains DELETE event') or dump_log("After temp pool undef");
}

# 5. EVICT event
TODO: {
    local $TODO = "Implement reliable eviction triggering for audit log verification";
    Protobuf::Internal::set_cache_capacity(2);
    # We have some items in cache. Let's add many to force eviction.
    for (1..10) {
        Protobuf::DescriptorPool->new();
    }
    $log = Protobuf::Internal::get_cache_audit_log();
    my @evicts = grep { $_->{type} == 5 } @$log;
    ok(scalar @evicts > 0, 'Log contains EVICT event') or dump_log("After capacity stress");
}

# Restore capacity
Protobuf::Internal::set_cache_capacity(100000);

# Check structure of an entry
my $entry = $log->[-1];
ok(exists $entry->{type}, 'Entry has type');
ok(exists $entry->{ptr}, 'Entry has ptr');
ok(exists $entry->{timestamp}, 'Entry has timestamp');

done_testing();
