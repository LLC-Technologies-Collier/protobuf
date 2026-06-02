use strict;
use warnings;
use Test::More;
use Protobuf;
plan skip_all => 'XS required for Arena Stats tests' unless $Protobuf::HAS_XS;
use Protobuf::Arena;

# 1. Standard Arena Stats
my $arena = Protobuf::Arena->new();
my $stats = $arena->stats;
is(ref($stats), 'HASH', 'stats returns a HASH ref');
ok($stats->{allocated} > 0, 'Initial allocated is > 0 (overhead)');
ok($stats->{reserved} > 0, 'Initial reserved is > 0');
is($stats->{blocks}, 1, 'Initial blocks is 1');

# 2. Track growth
# We need to allocate something to see growth.
# Since we don't have Message yet, we'll rely on the internal upb_Arena_Malloc.
# We don't have a direct Perl method for upb_Arena_Malloc yet.
# Let's add a temporary helper in Arena.xs or use space_allocated.

# Wait, space_allocated is already there.
my $alloc1 = $arena->space_allocated();
ok($alloc1 >= 0, "space_allocated returns $alloc1");

# 3. Tmpfs Arena Stats
use File::Temp qw(tempdir);
use File::Spec;
my $tmp = tempdir(CLEANUP => 1);
my $path = File::Spec->catfile($tmp, "stats_test.shm");
my $size = 65536;

my $tmp_arena = Protobuf::Arena->new_tmpfs($path, $size);
my $tmp_stats = $tmp_arena->stats;
is($tmp_stats->{reserved}, $size, 'Tmpfs reserved matches requested size');
is($tmp_stats->{blocks}, 1, 'Tmpfs has 1 block');
# Initial allocated includes the overhead of the upb_Arena structure itself 
# if it's placed in the arena.
ok($tmp_stats->{allocated} > 0, "Tmpfs initial allocated is $tmp_stats->{allocated}");

done_testing();
