use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use File::Temp qw(tempdir);
use File::Spec;

my $tmp = tempdir(CLEANUP => 1);
my $path = File::Spec->catfile($tmp, "test_arena.shm");
my $size = 1024 * 64; # 64KB

# 1. Create tmpfs arena
my $arena = Protobuf::Arena->new_tmpfs($path, $size);
ok($arena, "Created tmpfs arena at $path");
isa_ok($arena, "Protobuf::Arena");

# 2. Verify file exists and has correct size
ok(-f $path, "Tmpfs file exists");
is(-s $path, $size, "Tmpfs file has correct size");

# 3. IPC Simulation
# Write something to the arena and verify it stays in the file
# Since we don't have high-level message objects fully wired to use this arena yet in this test,
# we'll use a simple verification: we'll create a second arena pointing to the same file.

my $arena2 = Protobuf::Arena->new_tmpfs($path, $size);
ok($arena2, "Created second tmpfs arena on same file");

# Check if they share the same memory region by checking space allocated 
# (This is a bit indirect without raw memory access from Perl)
# For now, just ensuring they both can exist and don't crash is a good first step.

undef $arena;
undef $arena2;
ok(1, "Cleaned up both arenas");

done_testing();
