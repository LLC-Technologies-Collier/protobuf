use strict;
use warnings;
use Test::More;
use Protobuf;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/compat_descriptor.bin');

subtest 'Float/Double Special Values' => sub {
    my $msg = Protobuf::Types::Types->new();
    
    # 1. Infinity
    my $inf = 9**9**9;
    eval { $msg->set_opt_double($inf) };
    ok(!$@, 'Set Double Inf success');
    is($msg->opt_double, $inf, 'Value is Inf');
    
    # 2. NaN
    my $nan = $inf / $inf;
    eval { $msg->set_opt_double($nan) };
    ok(!$@, 'Set Double NaN success');
    ok(isnan($msg->opt_double), 'Value is NaN');
};

sub isnan {
    my ($val) = @_;
    return ($val != $val);
}

done_testing();
