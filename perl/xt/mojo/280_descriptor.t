package main;
use strict;
use warnings;
use Test::More;

BEGIN {
    plan skip_all => 'Mojolicious required for Mojo::IOLoop'
        unless eval { require Mojo::IOLoop; require Mojo::Promise; 1 };
}

use Protobuf::DescriptorPool;

# Load data once
my $file_path = 't/data/test_descriptor.bin';
open my $fh, '<:raw', $file_path or die "Could not open $file_path: $!";
my $data = do { local $/; <$fh> };
close $fh;

subtest 'Concurrent descriptor caching and access with Mojo::IOLoop' => sub {
    my $pool = Protobuf::DescriptorPool->new();
    $pool->add_serialized_file_descriptor_set($data);

    my $concurrency = 50;
    my @promises;

    for my $i (1..$concurrency) {
        my $p = Mojo::Promise->new();
        push @promises, $p;

        # Delay randomly up to 50ms using a timer
        Mojo::IOLoop->timer(rand(0.05) => sub {
            eval {
                # Test object cache consistency across the event loop
                my $mdef = $pool->find_message_by_name("protobuf_perl_test.TestMessage");
                die "mdef missing" unless $mdef;

                my $mdef_again = $pool->find_message_by_name("protobuf_perl_test.TestMessage");
                die "mdef identity mismatch in object cache" unless $mdef eq $mdef_again;

                my $fdef = $mdef->find_field_by_name("value");
                die "fdef missing" unless $fdef;

                $p->resolve();
            };
            if ($@) {
                $p->reject($@);
            }
        });
    }

    my $errors = 0;
    Mojo::Promise->all(@promises)->then(sub {
        pass("All $concurrency asynchronous operations completed successfully");
    })->catch(sub {
        my ($err) = @_;
        fail("Async operations failed: $err");
        $errors++;
    })->wait;

    is($errors, 0, "No errors encountered during Mojo::IOLoop execution");
};

done_testing();
