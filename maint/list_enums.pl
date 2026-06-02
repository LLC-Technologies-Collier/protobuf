use strict;
use warnings;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->new;
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die $!;
my $data = do { local $/; <$fh> };
close $fh;
$pool->add_serialized_file_descriptor_set($data);

my $edef = $pool->find_enum_by_name('test.TestEnum');
print "Enum: " . $edef->name . "\n";
for my $i (0 .. $edef->value_count - 1) {
    print "  $i: " . $edef->value_name($i) . "\n";
}
