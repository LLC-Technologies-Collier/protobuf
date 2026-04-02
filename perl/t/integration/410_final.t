package main;
use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'final integration' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(123);
    
    # Verify multiple bridge methods
    ok($msg->dependency_graph(), 'Dependency graph works');
    ok($msg->perf_profile(), 'Performance profile works');
    
    my $json = $msg->to_json_compact();
    ok($json, 'JSON compact works');
    
    my $text = $msg->to_text();
    ok($text, 'Text format works');
};

done_testing();
