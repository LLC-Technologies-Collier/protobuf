use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
eval { require Mojo::IOLoop; 1 } or plan skip_all => "Mojo::IOLoop not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Mojo-specific arena tests";
    ok(0, "Arena is Mojo-safe");
}

done_testing();
