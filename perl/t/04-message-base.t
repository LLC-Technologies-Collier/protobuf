use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;
use Protobuf::Message;

subtest 'base message functionality' => sub {
    # Load descriptors into the generated pool
    my $pool = TestHelpers->get_generated_pool();
    TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    
    subtest 'creation' => sub {
        my $msg = test::TestMessage->new();
        ok($msg, 'Created test::TestMessage');
        isa_ok($msg, 'test::TestMessage');
        isa_ok($msg, 'Protobuf::Message');
    };

    subtest 'get and set' => sub {
        my $msg = test::TestMessage->new();
        
        # Test scalar field
        $msg->set('value', 42);
        is($msg->get('value'), 42, 'Set and get value (int32)');
        
        # Test another scalar field
        $msg->set('optional_uint32', 123);
        is($msg->get('optional_uint32'), 123, 'Set and get optional_uint32');
    };
    
    subtest 'serialization and parsing' => sub {
        my $msg = test::TestMessage->new();
        $msg->set('value', 100);
        $msg->set('optional_uint32', 200);
        
        my $data = $msg->serialize();
        ok($data, 'Serialized message');
        
        my $msg2 = test::TestMessage->parse($data);
        ok($msg2, 'Parsed message');
        isa_ok($msg2, 'test::TestMessage');
        is($msg2->get('value'), 100, 'Correct value after parse');
        is($msg2->get('optional_uint32'), 200, 'Correct optional_uint32 after parse');
    };
};

TODO: {
    local $TODO = 'Implement Zero-Copy Field Access for large blobs';
    ok(0, 'String/Bytes accessors avoid memory copies via direct buffer SVs');
}

TODO: {
    local $TODO = 'Implement Moo-to-Native Method Bypass';
    ok(0, 'High-frequency accessors bypass Moo dispatch for ultra-fast performance');
}

TODO: {
    local $TODO = 'Provide Real-Time Message Validation Profiler';
    ok(0, 'Message types support fine-grained validation vs movement profiling');
}

done_testing();
