use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
eval { require Coro; 1 } or plan skip_all => "Coro not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Coro-specific arena tests";
    ok(0, "Arena is Coro-safe");
}

done_testing();
