use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

subtest 'load and explore descriptors' => sub {
    my $pool = TestHelpers->get_empty_pool();
    
    my $files = TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');
    ok($files, 'Added descriptor set');
    my $last_file = $files->[-1];
    isa_ok($last_file, 'Protobuf::Descriptor::File');
    is($last_file->get_package, 'test', 'Correct package');
    
    subtest 'message descriptors' => sub {
        my $mdef = $pool->find_message_by_name('test.TestMessage');
        ok($mdef, 'Found test.TestMessage');
        is($mdef->name, 'TestMessage', 'Correct name');
        
        my $f = $mdef->find_field_by_name('value');
        ok($f, 'Found field by name');
        is($f->number, 1, 'Correct number');
        is($f->type, 5, 'Type is int32 (5)'); # upb_CType_Int32
    };
    
    subtest 'enum descriptors' => sub {
        my $edef = $pool->find_enum_by_name('test.TestEnum');
        ok($edef, 'Found test.TestEnum');
        is($edef->name, 'TestEnum', 'Correct name');
        is($edef->value_count, 3, 'Correct value count');
    };
};

TODO: {
    local $TODO = 'Implement Direct-to-Native Reflection';
    ok(0, 'Internal tasks can access C-level defs bypassing Perl wrappers');
}

TODO: {
    local $TODO = 'Implement Descriptor-Level Memory Profile';
    ok(0, 'Each descriptor subclass provides memory overhead reporting');
}

TODO: {
    local $TODO = 'Implement Proto3-to-Proto2 Compatibility Layer';
    ok(0, 'Descriptors support cross-version compatibility mappings');
}

done_testing();
