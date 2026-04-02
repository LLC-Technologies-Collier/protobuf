package main;
use strict;
use warnings;
use Test::More;
eval { require Mojo::IOLoop; 1 } or plan skip_all => "Mojo::IOLoop not available";

plan tests => 1;

TODO: {
    local $TODO = "Implement Mojo-specific repeated field tests";
    ok(0, "RepeatedField is Mojo-safe");
}

done_testing();
