use strict;
use warnings;
use Test::More;
eval { require Coro; 1 } or plan skip_all => "Coro not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Coro-specific descriptor pool tests";
    ok(0, "DescriptorPool is Coro-safe");
}

done_testing();
