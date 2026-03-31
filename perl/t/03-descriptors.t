use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

subtest 'load and explore descriptors' => sub {
    my $pool = Protobuf::DescriptorPool->new;
    
    my $file_path = 't/data/test_descriptor.bin';
    open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
    my $data = do { local $/; <$fh> };
    close $fh;
    
    my $files = $pool->add_serialized_file_descriptor_set($data);
    ok($files, 'Added descriptor set');
    my $last_file = $files->[-1];
    isa_ok($last_file, 'Protobuf::Descriptor::File');
    is($last_file->package, 'test', 'Correct package');
    
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

done_testing();
