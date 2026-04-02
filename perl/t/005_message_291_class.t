use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use lib "t/lib";
use TestHelpers;
use Protobuf::Message;

my $pool = TestHelpers->get_empty_pool();

# This should trigger ClassGenerator for test.TestMessage and test.NestedMessage
my $file = TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
ok($file, 'Added file descriptor set');

# Check if classes were generated
ok(test::TestMessage->can('new'), 'test::TestMessage class was generated and can new()');
ok(test::NestedMessage->can('new'), 'test::NestedMessage class was generated and can new()');

# Instantiate
my $msg = test::TestMessage->new();
ok($msg, 'Instantiated test::TestMessage');
isa_ok($msg, 'test::TestMessage');
isa_ok($msg, 'Protobuf::Message');

done_testing();
