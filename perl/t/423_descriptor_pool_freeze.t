use strict;
use warnings;
use Test::More;
use Protobuf;
plan skip_all => 'XS required for descriptor pool freeze tests' unless $Protobuf::HAS_XS;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

subtest 'descriptor pool freeze' => sub {
    my $pool = Protobuf::DescriptorPool->new();
    ok(!$pool->is_frozen(), "Pool is not frozen initially");
    
    # 1. Load some protos
    TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    ok($pool->find_message_by_name('test.TestMessage'), "Found message before freeze");
    
    # 2. Freeze the pool
    ok($pool->freeze(), "Froze the pool");
    ok($pool->is_frozen(), "Pool reports is_frozen");
    
    # 3. Attempt to add more (should fail)
    eval {
        TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    };
    ok($@, "Failed to add protos to a frozen pool");
    like($@, qr/Cannot add file to a frozen DescriptorPool/, "Correct error message");
};

subtest 'descriptor identity stability' => sub {
    my $pool = Protobuf::DescriptorPool->new();
    TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    
    my $m1 = $pool->find_message_by_name('test.TestMessage');
    my $m2 = $pool->find_message_by_name('test.TestMessage');
    
    ok($m1 && $m2, "Found message descriptors");
    is($m1, $m2, "Identical message descriptors returned (identity stability)");
    
    my $f1 = $pool->find_file_by_name('perl/t/c/test.proto') || $pool->find_file_by_name('t/c/test.proto');
    my $f2 = $pool->find_file_by_name('perl/t/c/test.proto') || $pool->find_file_by_name('t/c/test.proto');
    is($f1, $f2, "Identical file descriptors returned");
};

done_testing();
