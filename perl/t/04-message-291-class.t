use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use lib "t/lib";
use TestHelpers;
use Protobuf::Message;

my $pool = TestHelpers->get_empty_pool();
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;

# This should trigger ClassGenerator for test.TestMessage and test.NestedMessage
my $file = $pool->add_serialized_file_descriptor_set($data);
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
