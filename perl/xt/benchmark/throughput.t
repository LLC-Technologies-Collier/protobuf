use strict;
use warnings;
use Test::More;
use Benchmark qw(:all);
use Protobuf;
use lib "t/lib";
use TestHelpers;
use Math::BigInt;
use feature 'state';

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'Throughput Benchmarking' => sub {
    my $msg = Test::Test::TestMessage->new();

    diag("Benchmarking Protobuf-Perl-UPB throughput...");

    # Redirect cmpthese output to diag so it shows up in prove -v
    my $results = capture_bench(sub {
        cmpthese(-3, {
            'set_scalar' => sub {
                $msg->set_value(123);
                $msg->set_name("hello world");
            },
            'get_scalar' => sub {
                my $a = $msg->value;
                my $b = $msg->name;
            },
            'serialize' => sub {
                my $data = $msg->serialize();
            },
            'parse' => sub {
                state $data = do {
                    my $m = Test::Test::TestMessage->new(value => 123, name => "hello");
                    $m->serialize();
                };
                my $msg2 = Test::Test::TestMessage->parse($data);
            },
            'to_perl' => sub {
                my $h = $msg->to_perl();
            },
            'from_perl' => sub {
                $msg->from_perl({ value => 456, name => "bench" });
            }
        });
    });
    
    diag($results);
    ok(1, "Throughput benchmarks completed");
};

sub capture_bench {
    my ($code) = @_;
    my $out = "";
    open my $fh, ">", \$out or die $!;
    my $old_fh = select $fh;
    $code->();
    select $old_fh;
    close $fh;
    return $out;
}

done_testing();
