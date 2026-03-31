use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/wkt_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'wrappers operations' => sub {
    my $d = google::protobuf::DoubleValue->new();
    $d->set_value(3.14);
    is($d->value, 3.14, 'DoubleValue works');
    
    my $f = google::protobuf::FloatValue->new();
    $f->set_value(2.71);
    # Float precision comparison
    ok(abs($f->value - 2.71) < 0.001, 'FloatValue works');
    
    my $i64 = google::protobuf::Int64Value->new();
    $i64->set_value(9000000000);
    is($i64->value, 9000000000, 'Int64Value works');
    
    my $ui64 = google::protobuf::UInt64Value->new();
    $ui64->set_value(9000000000);
    is($ui64->value, 9000000000, 'UInt64Value works');
    
    my $i32 = google::protobuf::Int32Value->new();
    $i32->set_value(-12345);
    is($i32->value, -12345, 'Int32Value works');
    
    my $ui32 = google::protobuf::UInt32Value->new();
    $ui32->set_value(12345);
    is($ui32->value, 12345, 'UInt32Value works');
    
    my $b = google::protobuf::BoolValue->new();
    $b->set_value(1);
    ok($b->value, 'BoolValue true works');
    $b->set_value(0);
    ok(!$b->value, 'BoolValue false works');
    
    my $s = google::protobuf::StringValue->new();
    $s->set_value("hello");
    is($s->value, "hello", 'StringValue works');
    
    my $by = google::protobuf::BytesValue->new();
    $by->set_value("bytes\x00data");
    is($by->value, "bytes\x00data", 'BytesValue works');
};

done_testing();
