use strict;
use warnings;

use Test::More;
use Protobuf::DescriptorPool;
use Protobuf::Descriptor;
use Protobuf::Descriptor::Message;
use Protobuf::Descriptor::File;
use Protobuf::Descriptor::Enum;
use Protobuf::Descriptor::EnumValue;
use Protobuf::Descriptor::Field;
use Protobuf::Descriptor::Oneof;
use Protobuf::Descriptor::Service;
use Protobuf::Descriptor::Method;
use Path::Tiny;

# Corresponds to values in upb/def.h
use constant {   # FieldDescriptor::Type
    UPB_TYPE_FLOAT  => 1,
    UPB_TYPE_DOUBLE => 2,
    UPB_TYPE_BOOL   => 3,
    UPB_TYPE_STRING => 4,
    UPB_TYPE_BYTES  => 5,
    UPB_TYPE_MESSAGE=> 6,
    UPB_TYPE_ENUM   => 7,
    UPB_TYPE_INT32  => 8,
    UPB_TYPE_UINT32 => 9,
    UPB_TYPE_INT64  => 10,
    UPB_TYPE_UINT64 => 11,
};

my $pool = Protobuf::DescriptorPool->new();
ok($pool, "Created DescriptorPool");

my $fds_file = path("t/c/test_fds.pb");
ok(-f $fds_file, "FileDescriptorSet file exists");

my $serialized_fds = $fds_file->slurp_raw();
ok($pool->load_serialized_descriptor_set($serialized_fds), "Loaded FileDescriptorSet");

my $msg_desc = $pool->find_message_by_name("test.TestMessage");
ok($msg_desc, "Found message by name: test.TestMessage");
isa_ok($msg_desc, "Protobuf::Descriptor::Message");
isa_ok($msg_desc, "Protobuf::Descriptor");

is($msg_desc->name(), "TestMessage", "Descriptor name ok");
is($msg_desc->full_name(), "test.TestMessage", "Descriptor full_name ok");
is($msg_desc->field_count(), 10, "Message field_count ok");
is($msg_desc->oneof_count(), 1, "Message oneof_count ok");

my $field_value = $msg_desc->field(0);
is($field_value->name(), "value", "Field index 0 name ok");
my $field_enum_field = $msg_desc->field(1);
is($field_enum_field->name(), "enum_field", "Field index 1 name ok");

my $file_desc = $msg_desc->pool->_wrap_file_def(Protobuf::C::MessageDescriptor::file($msg_desc->_upb_def));
ok($file_desc, "Got FileDescriptor from Message");
isa_ok($file_desc, "Protobuf::Descriptor::File");
is($file_desc->name(), "t/c/test.proto", "FileDescriptor name ok");
is($file_desc->package(), "test", "FileDescriptor package ok");

my $enum_desc = $pool->find_enum_by_name("test.TestEnum");
ok($enum_desc, "Found enum by name: test.TestEnum");
isa_ok($enum_desc, "Protobuf::Descriptor::Enum");
is($enum_desc->name(), "TestEnum", "EnumDescriptor name ok");
is($enum_desc->full_name(), "test.TestEnum", "EnumDescriptor full_name ok");
is($enum_desc->value_count(), 3, "EnumDescriptor value_count ok");

my $enum_val0 = $enum_desc->value(0);
isa_ok($enum_val0, "Protobuf::Descriptor::EnumValue");
is($enum_val0->name(), "TEST_ENUM_UNSPECIFIED", "EnumValue name ok");
is($enum_val0->number(), 0, "EnumValue number ok");
is($enum_val0->index(), 0, "EnumValue index ok");

my $enum_val1 = $enum_desc->value(1);
is($enum_val1->name(), "TEST_ENUM_FIRST", "EnumValue name ok");
is($enum_val1->number(), 1, "EnumValue number ok");
is($enum_val1->index(), 1, "EnumValue index ok");

my $enum_val2 = $enum_desc->value(2);
is($enum_val2->name(), "TEST_ENUM_SECOND", "EnumValue name ok");
is($enum_val2->number(), 2, "EnumValue number ok");
is($enum_val2->index(), 2, "EnumValue index ok");

# Test Fields
my $field0 = $pool->_wrap_field_def(Protobuf::C::MessageDescriptor::field($msg_desc->_upb_def, 0));
isa_ok($field0, "Protobuf::Descriptor::Field");
is($field0->name(), "value", "Field name ok");
is($field0->full_name(), "test.TestMessage.value", "Field full_name ok");
is($field0->type(), UPB_TYPE_INT32, "Field type ok");
is($field0->number(), 1, "Field number ok");
is($field0->index(), 0, "Field index ok");
isa_ok($field0->containing_type(), "Protobuf::Descriptor::Message");
is($field0->containing_type()->full_name(), "test.TestMessage", "Field containing_type ok");
ok(!$field0->message_type(), "Field message_type undef ok");
ok(!$field0->enum_type(), "Field enum_type undef ok");
ok($field0->is_optional(), "Field is_optional ok");
ok(!$field0->is_repeated(), "Field is_repeated false ok");
ok($field0->has_default(), "Field has_default ok");

my $field1 = $pool->_wrap_field_def(Protobuf::C::MessageDescriptor::field($msg_desc->_upb_def, 1));
isa_ok($field1, "Protobuf::Descriptor::Field");
is($field1->name(), "enum_field", "Field name ok");
is($field1->type(), UPB_TYPE_ENUM, "Field type ok");
ok(!$field1->message_type(), "Field message_type undef ok");
isa_ok($field1->enum_type(), "Protobuf::Descriptor::Enum");
is($field1->enum_type()->full_name(), "test.TestEnum", "Field enum_type ok");

# Repeated field
my $field_repeated = $msg_desc->field(5);
isa_ok($field_repeated, "Protobuf::Descriptor::Field");
is($field_repeated->name(), "repeated_int", "Repeated field name ok");
ok(!$field_repeated->is_optional(), "Repeated field is_optional false ok");
ok($field_repeated->is_repeated(), "Repeated field is_repeated ok");
ok(!$field_repeated->has_default(), "Repeated field has_default false ok");

# Test Oneofs
my $oneof_desc = $pool->_wrap_oneof_def(Protobuf::C::MessageDescriptor::oneof($msg_desc->_upb_def, 0));
isa_ok($oneof_desc, "Protobuf::Descriptor::Oneof");
is($oneof_desc->name(), "test_oneof", "Oneof name ok");
is($oneof_desc->full_name(), "test.TestMessage.test_oneof", "Oneof full_name ok");
is($oneof_desc->index(), 0, "Oneof index ok");
is($oneof_desc->field_count(), 2, "Oneof field_count ok");

my $oneof_field0 = $oneof_desc->field(0);
isa_ok($oneof_field0, "Protobuf::Descriptor::Field");
is($oneof_field0->name(), "oneof_string", "Oneof field name ok");

my $oneof_field1 = $oneof_desc->field(1);
isa_ok($oneof_field1, "Protobuf::Descriptor::Field");
is($oneof_field1->name(), "oneof_int32", "Oneof field name ok");

# Test Services
my $service_desc = $pool->find_service_by_name("test.TestService");
ok($service_desc, "Found service by name: test.TestService");
isa_ok($service_desc, "Protobuf::Descriptor::Service");
is($service_desc->name(), "TestService", "ServiceDescriptor name ok");
is($service_desc->full_name(), "test.TestService", "ServiceDescriptor full_name ok");
is($service_desc->method_count(), 1, "ServiceDescriptor method_count ok");

my $method_desc = $service_desc->method(0);
isa_ok($method_desc, "Protobuf::Descriptor::Method");
is($method_desc->name(), "TestRpc", "MethodDescriptor name ok");
is($method_desc->full_name(), "test.TestService.TestRpc", "MethodDescriptor full_name ok");
isa_ok($method_desc->input_type(), "Protobuf::Descriptor::Message");
is($method_desc->input_type()->full_name(), "test.TestMessage", "Method input_type ok");
isa_ok($method_desc->output_type(), "Protobuf::Descriptor::Message");
is($method_desc->output_type()->full_name(), "test.AnotherMessage", "Method output_type ok");

# Nested Enum
my $nested_enum = $pool->find_enum_by_name("test.TestMessage.NestedEnum");
ok($nested_enum, "Found nested enum");
isa_ok($nested_enum, "Protobuf::Descriptor::Enum");
is($nested_enum->full_name(), "test.TestMessage.NestedEnum", "Nested enum full name");

done_testing();
