use strict;
use warnings;
use Test::More;
use Config;

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
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/test_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

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

done_testing();
