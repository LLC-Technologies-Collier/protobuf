use strict;
use warnings;
use Test::More;
use Protobuf::DescriptorPool;

my $pool = Protobuf::DescriptorPool->generated_pool();
open my $fh, '<:raw', 't/data/wkt_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'duration operations' => sub {
    my $dur = google::protobuf::Duration->new();
    $dur->from_seconds(123.456);
    
    is($dur->seconds, 123, 'Seconds part correct');
    is($dur->nanos, 456_000_000, 'Nanos part correct');
    
    is($dur->to_seconds, 123.456, 'Conversion back to seconds matches');
};

done_testing();
