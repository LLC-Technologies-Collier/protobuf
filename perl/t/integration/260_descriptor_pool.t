use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;
use Protobuf::Arena;

plan tests => 1;

TODO: {
    local $TODO = "Implement DescriptorPool/Arena integration tests";
    ok(0, "DescriptorPool and Arena interact correctly");
}

done_testing();
