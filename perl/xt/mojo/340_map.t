use strict;
use warnings;
use Test::More;
eval { require Mojo::IOLoop; 1 } or plan skip_all => "Mojo::IOLoop not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Mojo-specific map tests";
    ok(0, "Map is Mojo-safe");
}

done_testing();
