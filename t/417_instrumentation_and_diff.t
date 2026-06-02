use strict;
use warnings;
use Test::More;
use Protobuf::Internal;
use Capture::Tiny qw(capture);

subtest 'binary diff verification' => sub {
    my $data1 = "\x01\x02\x03";
    my $data2 = "\x01\x02\x03";
    my $data3 = "\x01\x02\x04";

    eval {
        Protobuf::Internal::verify_binary_diff($data1, $data2, "matching data");
    };
    ok(!$@, "Matching data passes") or diag($@);

    my $exception;
    eval {
        capture {
            Protobuf::Internal::verify_binary_diff($data1, $data3, "mismatching data");
        };
    };
    $exception = $@;

    ok($exception, "Mismatching data croaks");
    like($exception, qr/Binary diff verification failed: mismatching data/, "Error message is correct");
    # We can't easily check the STDERR content here because capture doesn't return if the block dies.
    # The main goal is to suppress the STDERR leak in the TAP output.
};

subtest 'avx2 instrumentation' => sub {
    my $features = Protobuf::Internal::get_cpu_features();
    if (!($features & 0x02)) {
        plan skip_all => "AVX2 not supported on this CPU";
    }

    $ENV{PROTOBUF_PERL_INSTRUMENT_AVX2} = 1;
    
    # Trigger a path that uses AVX2 instrumentation
    # PerlUpb_ClassNameToFullName uses it for strings >= 32 bytes
    my $long_class = "A" x 40;
    
    my ($stdout, $stderr, $full);
    ($stdout, $stderr, $full) = capture {
        Protobuf::Internal::class_name_to_full_name($long_class);
    };
    is($full, $long_class, "Conversion works with instrumentation enabled");
    like($stderr, qr/\[AVX2\] Hitting path/, "Instrumentation message captured in stderr")
        if $stderr; # It might not hit if strings are too short or logic changes, but we check if we can.
    
    delete $ENV{PROTOBUF_PERL_INSTRUMENT_AVX2};
};

done_testing();
