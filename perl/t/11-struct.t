use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/wkt_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'struct and value operations' => sub {
    my $data = {
        foo => 'bar',
        baz => 123,
        qux => [1, 2, { a => 'b' }],
        sub => { x => 1 }
    };
    
    my $struct = google::protobuf::Struct->new();
    $struct->from_perl($data);
    
    my $out = $struct->to_perl();
    is_deeply($out, $data, 'Struct from_perl/to_perl roundtrip matches');
    
    # Check individual fields
    is($struct->fields->{foo}->string_value, 'bar', 'String value correct');
    is($struct->fields->{baz}->number_value, 123, 'Number value correct');
    
    # Check oneof
    is($struct->fields->{foo}->kind, 'string_value', 'Oneof kind is string_value');
};

done_testing();
