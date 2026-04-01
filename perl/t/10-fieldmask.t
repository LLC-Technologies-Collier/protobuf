use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
open my $fh, '<:raw', 't/data/wkt_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'fieldmask operations' => sub {
    my $mask = google::protobuf::FieldMask->new();
    
    # Adding paths
    push @{$mask->paths}, "foo.bar", "baz";
    
    is_deeply($mask->paths, ["foo.bar", "baz"], 'Paths array matches');
    
    # Optional helper methods, if we implemented them
    if ($mask->can('to_string')) {
        is($mask->to_string, 'foo.bar,baz', 'String representation matches');
    }
    if ($mask->can('from_string')) {
        my $m2 = google::protobuf::FieldMask->new();
        $m2->from_string('a,b.c');
        is_deeply($m2->paths, ['a', 'b.c'], 'Parsed from string');
    }
};

done_testing();
