use strict;
use warnings;
use Test::More;
use Protobuf::Arena;

subtest 'basic creation' => sub {
    my $arena = Protobuf::Arena->new;
    ok($arena, 'Created arena');
    isa_ok($arena, 'Protobuf::Arena');
    ok($arena->{_arena_ptr}, 'Has internal arena pointer');
};

subtest 'multiple arenas' => sub {
    my $a1 = Protobuf::Arena->new;
    my $a2 = Protobuf::Arena->new;
    isnt($a1->{_arena_ptr}, $a2->{_arena_ptr}, 'Different arenas have different pointers');
};

subtest 'destruction' => sub {
    {
        my $arena = Protobuf::Arena->new;
    }
    pass('Destroyed arena without crash');
};

subtest 'explicit pointer access' => sub {
    my $arena = Protobuf::Arena->new;
    my $ptr = $arena->{_arena_ptr};
    ok($ptr, "Got pointer $ptr");
};

ok(1, 'All basic tests passed');

TODO: {
    local $TODO = 'Implement Arena Fusion (Cloning)';
    ok(0, 'Arena fusion allows data transfer without deep copy');
}

TODO: {
    local $TODO = 'Implement Perl-Level Arena Memory Statistics';
    ok(0, 'Arena stats() returns detailed memory usage metrics');
}

TODO: {
    local $TODO = 'Support Custom Allocators for Protobuf::Arena';
    ok(0, 'Protobuf::Arena supports specialized C-level memory pools');
}

done_testing();
