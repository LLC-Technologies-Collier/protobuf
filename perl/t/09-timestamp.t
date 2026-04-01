use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;
use Time::Piece;

my $pool = TestHelpers->get_generated_pool();
open my $fh, '<:raw', 't/data/wkt_descriptor.bin' or die $!;
my $data = do { local $/; <$fh> };
$pool->add_serialized_file_descriptor_set($data);

subtest 'timestamp operations' => sub {
    my $ts = google::protobuf::Timestamp->new();
    my $now = gmtime();
    
    $ts->from_time_piece($now);
    is($ts->seconds, $now->epoch, 'Epoch matches');
    
    my $tp = $ts->to_time_piece();
    is($tp->epoch, $now->epoch, 'Time::Piece back-conversion matches');
    
    # ISO8601
    my $iso = $ts->to_iso8601();
    like($iso, qr/^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$/, 'ISO8601 format correct');
};

done_testing();
