use strict;
use warnings;
use Test::More;
use Protobuf;
use lib "t/lib";
use TestHelpers;
use File::Temp qw(tempfile);

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'Handle Serialization' => sub {
    my $msg = Test::Test::TestMessage->new(
        value => 12345,
        name => "handle test"
    );
    
    # 1. to_handle
    my ($fh, $filename) = tempfile();
    eval { $msg->to_handle($fh) };
    ok(!$@, "to_handle success") or diag($@);
    close $fh;
    
    # Verify file content
    open my $rh, "<:raw", $filename or die $!;
    my $content = do { local $/; <$rh> };
    close $rh;
    
    is($content, $msg->serialize(), "File content matches standard serialization");
    
    # 2. from_handle
    open my $rh2, "<:raw", $filename or die $!;
    my $msg2 = eval { Test::Test::TestMessage->from_handle($rh2) };
    ok(!$@, "from_handle success") or diag($@);
    close $rh2;
    
    ok($msg2, "Parsed message from handle");
    isa_ok($msg2, 'Test::Test::TestMessage');
    is($msg2->value, 12345, "Value matches");
    is($msg2->name, "handle test", "Name matches");
    
    unlink($filename);
};

subtest 'Length-Prefixed Streaming' => sub {
    my ($fh, $filename) = tempfile();
    binmode($fh);
    
    my @msgs = (
        Test::Test::TestMessage->new(value => 1, name => "one"),
        Test::Test::TestMessage->new(value => 2, name => "two"),
        Test::Test::TestMessage->new(value => 3, name => "three"),
    );
    
    # Write all messages
    foreach my $m (@msgs) {
        $m->to_handle($fh, length_prefixed => 1);
    }
    close $fh;
    
    # Read them back
    open my $rh, "<:raw", $filename or die $!;
    my @parsed;
    while (1) {
        my $m = Test::Test::TestMessage->from_handle($rh, length_prefixed => 1);
        last unless defined $m;
        push @parsed, $m;
    }
    close $rh;
    
    is(scalar @parsed, 3, "Read back 3 messages");
    is($parsed[0]->name, "one", "First message matches");
    is($parsed[1]->name, "two", "Second message matches");
    is($parsed[2]->name, "three", "Third message matches");
    
    unlink($filename);
};

subtest 'JSON Handle Serialization' => sub {
    my $msg = Test::Test::TestMessage->new(value => 42, name => "json handle");
    my ($fh, $filename) = tempfile();
    eval { $msg->to_handle($fh, format => 'json') };
    ok(!$@, "to_handle json success") or diag($@);
    close $fh;
    
    open my $rh, "<", $filename or die $!;
    my $content = do { local $/; <$rh> };
    close $rh;
    
    like($content, qr/\"value\":\s*42/, "JSON content contains value");
    like($content, qr/\"name\":\s*\"json handle\"/, "JSON content contains name");
    is($content, $msg->to_json(), "JSON content matches to_json output");
    
    unlink($filename);
};

subtest 'Handle Errors' => sub {
    my $msg = Test::Test::TestMessage->new();
    
    eval { $msg->to_handle(undef) };
    like($@, qr/Invalid file handle/i, "to_handle with undef fails");
    
    eval { Test::Test::TestMessage->from_handle(undef) };
    like($@, qr/Invalid file handle/i, "from_handle with undef fails");
};

done_testing();
