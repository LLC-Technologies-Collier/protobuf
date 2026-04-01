use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;
use POSIX qw( mkfifo );
use File::Temp qw( tempdir );

my $pool = TestHelpers->get_generated_pool();
foreach my $file ('t/data/test_descriptor.bin') {
    open my $fh, '<:raw', $file or die $!;
    my $data = do { local $/; <$fh> };
    $pool->add_serialized_file_descriptor_set($data);
}

my $tmpdir = tempdir( CLEANUP => 1 );
my $shm_file = "$tmpdir/proto_shm";
my $fifo = "$tmpdir/proto_fifo";

mkfifo($fifo, 0600) or die "mkfifo failed: $!";

my $pid = fork();
die "fork failed" unless defined $pid;

if ($pid == 0) {
    # Child Process: Consumer
    # 1. Wait for signal from FIFO
    open my $fh_fifo, '<', $fifo or die $!;
    my $line = <$fh_fifo>;
    close $fh_fifo;
    
    # 2. Read from SHM file
    open my $fh_shm, '<:raw', $shm_file or die $!;
    my $data = do { local $/; <$fh_shm> };
    close $fh_shm;
    
    # 3. Parse and validate
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->parse($data);
    if ($msg->optional_int32 == 12345 && $msg->optional_string eq "shm_test") {
        # Signal success back via exit code
        exit 0;
    } else {
        exit 1;
    }
} else {
    # Parent Process: Producer
    # 1. Create message
    my $msg = protobuf_test_messages::proto2::TestAllTypesProto2->new();
    $msg->set_optional_int32(12345);
    $msg->set_optional_string("shm_test");
    
    # 2. Write to SHM file
    my $wire = $msg->serialize();
    open my $fh_shm, '>:raw', $shm_file or die $!;
    print $fh_shm $wire;
    close $fh_shm;
    
    # 3. Signal via FIFO
    open my $fh_fifo, '>', $fifo or die $!;
    print $fh_fifo "READY\n";
    close $fh_fifo;
    
    # 4. Wait for child
    waitpid($pid, 0);
    my $exit_code = $? >> 8;
    
    is($exit_code, 0, 'Consumer parsed message from SHM correctly');
}

done_testing();
