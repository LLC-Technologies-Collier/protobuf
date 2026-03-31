use strict;
use warnings;
use Test::More;
use Protobuf::Arena;
use Protobuf::DescriptorPool;

subtest 'descriptor pool and arena interaction' => sub {
    my $arena = Protobuf::Arena->new;
    ok($arena, 'Created arena');

    my $pool = Protobuf::DescriptorPool->new;
    ok($pool, 'Created descriptor pool');

    my $file_path = 't/data/test_descriptor.bin';
    open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
    my $data = do { local $/; <$fh> };
    close $fh;

    my $last_file = $pool->add_serialized_file_descriptor_set($data);
    ok($last_file, 'Added serialized file descriptor set');

    my $message_def = $pool->find_message_by_name('test.TestMessage');
    ok($message_def, 'Found test.TestMessage in pool');
    isa_ok($message_def, 'Protobuf::Descriptor::MessageDef');

    # The MessageDef should stay alive even if we undef the pool because
    # the pool itself holds the underlying C memory, but the message_def
    # wrapper contains a weak reference to it (or rather, the Perl object cache
    # ties them together). Wait, how does the user API dictate this?
    # Right now, our wrapper objects have SV pointers. Let's make sure
    # we don't crash when passing these things around.

    is($message_def->name, 'TestMessage', 'Message name is correct');
    is($message_def->full_name, 'test.TestMessage', 'Message full_name is correct');

    # Verify we can find fields
    my $field = $message_def->find_field_by_name('value');
    ok($field, 'Found field "value"');
    isa_ok($field, 'Protobuf::Descriptor::Field');
    is($field->name, 'value', 'Field name is correct');

    # Verify Enum types
    my $enum_def = $pool->find_enum_by_name('test.TestEnum');
    ok($enum_def, 'Found enum test.TestEnum');
    isa_ok($enum_def, 'Protobuf::Descriptor::Enum');
    is($enum_def->name, 'TestEnum', 'Enum name is correct');
};

done_testing();
