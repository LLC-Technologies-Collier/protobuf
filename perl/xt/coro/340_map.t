use strict;
use warnings;
use Test::More;
eval { require Coro; 1 } or plan skip_all => "Coro not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Coro-specific map tests";
    ok(0, "Map is Coro-safe");
}

done_testing();
