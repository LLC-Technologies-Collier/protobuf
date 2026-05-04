use strict;
use warnings;
use Test::More;
use Time::HiRes qw(gettimeofday tv_interval);
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

# Define hard baselines (seconds for 100 iterations of 10000 elements)
use constant BASELINE_UPBTOSV => 0.05; 
use constant BASELINE_SVTOUPB => 0.05;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
my $mdef = $pool->find_message_by_name('test.TestMessage');

my $count = 10000;
my @data = (1..$count);

subtest 'performance guard: UpbToSv' => sub {
    my $msg = Test::Test::TestMessage->new();
    $msg->repeated_int(\@data);
    
    my $t0 = [gettimeofday];
    for (1..100) {
        my $perl = $msg->to_perl();
    }
    my $elapsed = tv_interval($t0);
    
    diag("Current UpbToSv: ${elapsed}s (Baseline: " . BASELINE_UPBTOSV . "s)");
    ok($elapsed < BASELINE_UPBTOSV, "Performance within regression limits");
};

subtest 'performance guard: SvToUpb' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    my $t0 = [gettimeofday];
    for (1..100) {
        $msg->repeated_int(\@data);
    }
    my $elapsed = tv_interval($t0);
    
    diag("Current SvToUpb: ${elapsed}s (Baseline: " . BASELINE_SVTOUPB . "s)");
    ok($elapsed < BASELINE_SVTOUPB, "Performance within regression limits");
};

done_testing();
