package main;
use strict;
use warnings;
use Test::More;
use Time::HiRes qw(gettimeofday tv_interval);
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'basic benchmarks' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    $msg->set_name("benchmark test");
    
    my $iterations = 1000;
    
    # 1. Serialization
    my $t0 = [gettimeofday];
    for (1..$iterations) {
        my $wire = $msg->serialize();
    }
    my $elapsed = tv_interval($t0);
    diag(sprintf("Serialization (%d iterations): %.4fs (%.2f ops/sec)", $iterations, $elapsed, $iterations/$elapsed));
    
    # 2. Deserialization
    my $wire = $msg->serialize();
    my $t1 = [gettimeofday];
    for (1..$iterations) {
        my $parsed = test::TestMessage->parse($wire);
    }
    my $elapsed2 = tv_interval($t1);
    diag(sprintf("Deserialization (%d iterations): %.4fs (%.2f ops/sec)", $iterations, $elapsed2, $iterations/$elapsed2));
    
    ok(1, 'Benchmarks completed');
};

done_testing();
