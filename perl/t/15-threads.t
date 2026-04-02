use strict;
use warnings;
use Test::More;
use Config;
use Protobuf::DescriptorPool;

BEGIN {
    if (!$Config{useithreads}) {
        plan skip_all => "Perl not compiled with 'useithreads'";
    }
}

use File::Temp qw(tempfile);

subtest 'Protobuf objects croak on thread creation' => sub {
    my $code = <<'EOF';
use strict;
use warnings;
use threads;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

my $msg = test::TestMessage->new();
$msg->set_value(123);

# This should crash the process due to CLONE dying
my $t = threads->create(sub { return 1; });
$t->join() if $t;
EOF

    my ($fh, $filename) = tempfile();
    print $fh $code;
    close $fh;

    my $cmd = "$^X -Iblib/lib -Iblib/arch -Ilib $filename 2>&1";
    my $err_output = `$cmd`;
    my $exit_code = $? >> 8;
    
    unlink $filename;
    
    ok($exit_code != 0, "Process exited with non-zero code ($exit_code) due to CLONE exception");
    like($err_output, qr/Protobuf objects cannot be safely cloned across ithreads/, 'Stderr contains the custom croak message');
};

subtest 'pool freeze' => sub {
    my $pool = Protobuf::DescriptorPool->new();
    ok($pool->freeze(), 'Freeze pool works (skeletal)');
};

TODO: {
    local $TODO = 'Implement Thread-Safe Global Freezing';
    ok(0, 'Frozen DescriptorPools can be safely shared across ithreads');
}

TODO: {
    local $TODO = 'Implement Cross-Thread Message Serialization Handoff';
    ok(0, 'High-level utility handles safe message transfer between threads');
}

TODO: {
    local $TODO = 'Integrate ThreadSanitizer (TSan) for race detection';
    ok(0, 'System-wide data races are automatically detected during concurrent tests');
}

done_testing();
