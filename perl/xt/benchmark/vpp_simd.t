use strict;
use warnings;
use Test::More;
use Benchmark qw(:all);
use Time::HiRes qw(gettimeofday tv_interval);
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

# 1. Setup large message with repeated fields
my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
my $mdef = $pool->find_message_by_name('protobuf_perl_test.TestMessage');

my $count = 10000;
my @data = (1..$count);

# 2. Benchmark to_perl (UpbToSv) with VPP fast-path
subtest 'VPP SIMD Benchmark: UpbToSv' => sub {
    my $msg = Protobuf_perl_test::Test::TestMessage->new();
    $msg->repeated_int(\@data);

    my $t0 = [gettimeofday];
    for (1..100) {
        my $perl = $msg->to_perl();
    }
    my $elapsed = tv_interval($t0);

    diag("VPP Fast-path UpbToSv: ${elapsed}s for 100 iterations of $count elements");
    ok($elapsed < 1.0, "Meets performance baseline for SIMD batch conversion");
};

# 3. Benchmark from_perl (SvToUpb) with VPP fast-path
subtest 'VPP SIMD Benchmark: SvToUpb' => sub {
    my $msg = Protobuf_perl_test::Test::TestMessage->new();

    my $t0 = [gettimeofday];
    for (1..100) {
        $msg->repeated_int(\@data);
    }
    my $elapsed = tv_interval($t0);

    diag("VPP Fast-path SvToUpb: ${elapsed}s for 100 iterations of $count elements");
    ok($elapsed < 1.0, "Meets performance baseline for SIMD batch population");
};

done_testing();
