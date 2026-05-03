use strict;
use warnings;
use Test::More;
use Math::BigInt;
use Protobuf::Arena;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'Math::BigInt Roundtrip in Integrated Message' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    # Use a large positive int64 value.
    # On many 64-bit systems, 9223372036854775807 (INT64_MAX) fits in a native IV
    # and might be returned as a simple scalar string or IV depending on internal
    # representation.
    # Let's test with something that is definitely BigInt-worthy if we want to force it,
    # OR we accept that on 64-bit Perls it might just be a regular scalar.
    
    my $val_str = "9223372036854775807"; # INT64_MAX
    my $bi = Math::BigInt->new($val_str);
    
    $msg->set_optional_int64($bi);
    my $got = $msg->optional_int64;
    
    # On 64-bit Perls, this might NOT be a reference if it fits in IV.
    # Our implementation prefers native IVs for performance if they fit.
    if (ref($got)) {
        ok(ref($got), "Returned value is a reference (Math::BigInt)");
        is($got->bstr(), $val_str, "Value matches original string after roundtrip via Message accessors");
    } else {
        ok(1, "Returned value is a native scalar (expected on 64-bit IV systems)");
        is($got . "", $val_str, "Value matches original string");
    }
    
    # Serialization roundtrip
    my $data = $msg->serialize();
    my $msg2 = Test::Test::TestMessage->parse($data);
    my $got2 = $msg2->optional_int64;
    
    if (ref($got2)) {
        is($got2->bstr(), $val_str, "Value matches after serialization roundtrip (BigInt)");
    } else {
        is($got2 . "", $val_str, "Value matches after serialization roundtrip (native)");
    }
};

subtest 'Math::BigInt Small Value Optimization' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    # Small value that fits in IV should NOT be a BigInt on retrieval
    $msg->set_optional_int64(Math::BigInt->new(12345));
    my $got = $msg->optional_int64;
    
    ok(!ref($got), "Small value returned as native scalar, not BigInt object");
    is($got, 12345, "Small value is correct");
};

done_testing();
