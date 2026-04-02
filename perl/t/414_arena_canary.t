use strict;
use warnings;
use Test::More;
use Protobuf::Arena;

# 1. Successful Arena Cleanup (No corruption)
{
    my $arena = Protobuf::Arena->new();
    ok($arena, "Created arena");
    # No corruption, should cleanup silently
}
ok(1, "Arena cleaned up successfully without corruption");

# 2. Detected Corruption
{
    use File::Temp qw(tempdir);
    use File::Spec;
    my $tmp = tempdir(CLEANUP => 1);
    my $path = File::Spec->catfile($tmp, "canary_test.shm");
    my $size = 16384;

    my $arena = Protobuf::Arena->new_tmpfs($path, $size);
    
    my $ok = eval {
        $arena->__test_block_canary();
        1;
    };
    my $err = $@;
    
    ok(!$ok, "Arena operation failed as expected due to corruption");
    like($err, qr/MEMORY CORRUPTION DETECTED/, "Error message contains expected string");
}

done_testing();
