#!/usr/bin/perl
use strict;
use warnings;
use Test::More;
use Benchmark qw(:all);
use lib "t/lib";
use TestHelpers;
use File::Temp qw(tempdir);
use Path::Tiny;
use File::Spec;

# Core Protobuf modules
use Protobuf;
use Protobuf::ClassGenerator;

plan skip_all => "Author benchmarks" unless $ENV{AUTHOR_TESTING} || $ENV{BENCHMARK};

# Setup shared Proto
my $proto_content = <<'EOF';
syntax = "proto3";
package zero_copy_bench;
message Data {
  string large_string = 1;
}
EOF

my $tmpdir = tempdir(CLEANUP => 1);
my $proto_file = File::Spec->catfile($tmpdir, "bench.proto");
path($proto_file)->spew($proto_content);
my $descriptor_bin = `protoc -I$tmpdir --descriptor_set_out=/dev/stdout $proto_file`;
die "protoc failed" if $? != 0;

my $pool = Protobuf::DescriptorPool->generated_pool();
$pool->add_serialized_file_descriptor_set($descriptor_bin);
my $file = $pool->find_file_by_name("bench.proto");
my $class = $file->get_top_level_message(0)->perl_class_name;
Protobuf::ClassGenerator->generate_for_file($file);

subtest 'Zero Copy Parsing' => sub {
    my $large_payload = "A" x 1024; # 1KB string
    my $msg = $class->new({ large_string => $large_payload });
    my $data = $msg->serialize();

    note(sprintf("--- Parsing Performance (Payload size: %d bytes) ---", length($data)));

    cmpthese(-3, {
        '01_copying_parse' => sub {
            my $m = $class->parse($data, { profile => 'balanced' });
            my $s = $m->large_string;
        },
        '02_zero_copy_parse' => sub {
            my $m = $class->parse($data, { profile => 'zero_copy' });
            my $s = $m->large_string;
        },
    });

    ok(1, "Benchmarks completed");
};

done_testing();
