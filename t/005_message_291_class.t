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
ok(Test::Test::TestMessage->can('new'), 'Test::Test::TestMessage class was generated and can new()');
ok(Test::Test::NestedMessage->can('new'), 'Test::Test::NestedMessage class was generated and can new()');

# Instantiate
my $msg = Test::Test::TestMessage->new();
ok($msg, 'Instantiated Test::Test::TestMessage');
isa_ok($msg, 'Test::Test::TestMessage');
isa_ok($msg, 'Protobuf::Message');

done_testing();
