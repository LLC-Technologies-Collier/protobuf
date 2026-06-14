#!/usr/bin/perl
use strict;
use warnings;
use Test::More;
use Time::HiRes qw(gettimeofday tv_interval);
use lib "t/lib";
use TestHelpers;
use File::Temp qw(tempdir);
use Path::Tiny;
use File::Spec;

# Core Protobuf modules
use Protobuf;
use Protobuf::ClassGenerator;

# Event-driven module
use Coro;

plan skip_all => "Author benchmarks" unless $ENV{AUTHOR_TESTING} || $ENV{BENCHMARK};

# Setup shared Proto
my $proto_content = <<'EOF';
syntax = "proto3";
package coro_bench;
message Data {
  int32 id = 1;
  string name = 2;
  repeated int32 tags = 3;
}
EOF

my $tmpdir = tempdir(CLEANUP => 1);
my $proto_file = File::Spec->catfile($tmpdir, "bench.proto");
path($proto_file)->spew($proto_content);
my $descriptor_bin = `protoc -I$tmpdir --descriptor_set_out=/dev/stdout $proto_file`;
die "protoc failed" if $? != 0;

my $pool = Protobuf::DescriptorPool->generated_pool();
my $files = $pool->add_serialized_file_descriptor_set($descriptor_bin);
my $file = $files->[0];
my $mdef = $file->get_top_level_message(0);
my $class = $mdef->perl_class_name;

if (!$class->can('new')) {
    Protobuf::ClassGenerator->generate_for_file($file);
}

my $iterations = 100_000;
my $concurrency = 10;

subtest 'Coro Throughput Benchmark' => sub {
    my $msg = $class->new(id => 1, name => "coro test", tags => [1..10]);

    my $t0 = [gettimeofday];
    my @coros;
    for (1..$concurrency) {
        push @coros, async {
            for (1..($iterations/$concurrency)) {
                # Test getter (bare name), setter (set_ prefix), and serialization
                my $id = $msg->id();
                $msg->set_id($id + 1);
                my $data = $msg->serialize();

                if ($_ % 100 == 0) {
                    cede;
                }
            }
        };
    }
    $_->join for @coros;
    my $elapsed = tv_interval($t0);
    diag(sprintf("Coro (%d items, %d concurrency): %.4fs (%.2f ops/sec)",
        $iterations, $concurrency, $elapsed, $iterations/$elapsed));
    ok(1, "Coro benchmark finished");
};

done_testing();
