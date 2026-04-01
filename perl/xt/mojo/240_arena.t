use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
eval { require Mojo::IOLoop; 1 } or plan skip_all => "Mojo::IOLoop not available";

plan tests => 3;

TODO: {
    local $TODO = "Implement Mojo-specific arena tests";
    ok(0, 'Arena is Mojo-safe');
}

TODO: {
    local $TODO = 'Integrate Arena stats with Mojo::Log';
    ok(0, 'Real-time arena memory usage is reportable via Mojo log infrastructure');
}

TODO: {
    local $TODO = 'Stress Mojo IOLoop with high-frequency arena allocation';
    ok(0, 'System remains stable during 10,000 arena creations in IOLoop callbacks');
}


done_testing();
