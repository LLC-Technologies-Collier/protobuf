package main;
use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
eval { require Coro; 1 } or plan skip_all => "Coro not available";

plan tests => 3;

TODO: {
    local $TODO = "Implement Coro-specific arena tests";
    ok(0, 'Arena is Coro-safe');
}

TODO: {
    local $TODO = 'Stress concurrent arena allocation across 1000 coroutines';
    ok(0, 'System remains stable under massive Coro arena pressure');
}

TODO: {
    local $TODO = 'Verify arena migration from Coro to Mojo';
    ok(0, 'Arena created in Coro is safely usable in Mojo callback');
}


done_testing();
