use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;
use Protobuf::Internal;
use Protobuf::Arena;

subtest 'audit log: allocation tracking' => sub {
    # 1. Clear audit log (implicitly by getting it and checking start state)
    my $initial_log = Protobuf::Internal::get_cache_audit_log();
    
    # 2. Create an arena and perform some work
    {
        my $arena = Protobuf::Arena->new();
        # upb_Arena_Init will perform at least one allocation
    }
    # Arena is now destroyed, should have logged a free
    
    my $log = Protobuf::Internal::get_cache_audit_log();
    vdiag("Audit log size: " . scalar(@$log));
    
    my $mallocs = grep { $_->{type} == 10 } @$log;
    my $frees   = grep { $_->{type} == 11 } @$log;
    
    vdiag("MALLOCs: $mallocs, FREEs: $frees");
    
    ok($mallocs > 0, "Recorded at least one MALLOC");
    ok($frees > 0, "Recorded at least one FREE");
    is($mallocs, $frees, "Allocations and frees are balanced for short-lived arena");
};

subtest 'audit log: leak detection simulation' => sub {
    my $mallocs_before = grep { $_->{type} == 10 } @{Protobuf::Internal::get_cache_audit_log()};
    
    my $leaked_arena = Protobuf::Arena->new();
    # We don't let it go out of scope yet
    
    my $log = Protobuf::Internal::get_cache_audit_log();
    my $mallocs_after = grep { $_->{type} == 10 } @$log;
    my $frees_after   = grep { $_->{type} == 11 } @$log;
    
    ok($mallocs_after > $frees_after, "Detected unbalanced allocations (potential leak)");
    
    undef $leaked_arena;
    
    my $final_log = Protobuf::Internal::get_cache_audit_log();
    my $m_final = grep { $_->{type} == 10 } @$final_log;
    my $f_final = grep { $_->{type} == 11 } @$final_log;
    is($m_final, $f_final, "Leak resolved after manual cleanup");
};

done_testing();
