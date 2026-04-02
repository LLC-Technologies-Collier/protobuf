use strict;
use warnings;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->new();
my $desc_file = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $desc_file or die "Can't open $desc_file: $!";
my $serialized = do { local $/; <$fh> };
close $fh;

my $files = $pool->add_serialized_file_descriptor_set($serialized);
foreach my $f (@$files) {
    print "File: " . $f->name . "\n";
}
