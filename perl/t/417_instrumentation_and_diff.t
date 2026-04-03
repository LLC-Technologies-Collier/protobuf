use strict;
use warnings;
use Test::More;
use Protobuf::Internal;

subtest 'binary diff verification' => sub {
    my $data1 = "\x01\x02\x03";
    my $data2 = "\x01\x02\x03";
    my $data3 = "\x01\x02\x04";

    eval {
        Protobuf::Internal::verify_binary_diff($data1, $data2, "matching data");
    };
    ok(!$@, "Matching data passes") or diag($@);

    eval {
        Protobuf::Internal::verify_binary_diff($data1, $data3, "mismatching data");
    };
    ok($@, "Mismatching data croaks");
    like($@, qr/Binary diff verification failed: mismatching data/, "Error message is correct");
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
    
    # We can't easily capture stderr here without more complex setup, 
    # but we can verify it doesn't crash.
    my $full = Protobuf::Internal::class_name_to_full_name($long_class);
    is($full, $long_class, "Conversion works with instrumentation enabled");
    
    delete $ENV{PROTOBUF_PERL_INSTRUMENT_AVX2};
};

done_testing();
