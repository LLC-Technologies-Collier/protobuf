use strict;
use warnings;
use Protobuf::DescriptorPool;
use Protobuf::ClassGenerator;

my $pool = Protobuf::DescriptorPool->new;
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die $!;
my $data = do { local $/; <$fh> };
close $fh;

my $file = $pool->add_serialized_file_descriptor_set($data);
print "File returned? " . ($file ? "yes" : "no") . "\n";
print "Can value? " . (test::TestMessage->can('value') ? "yes" : "no") . "\n";
