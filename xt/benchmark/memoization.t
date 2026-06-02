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
package memo_bench;
message Data {
  int32 id = 1;
  string name = 2;
}
EOF

my $tmpdir = tempdir(CLEANUP => 1);
my $proto_file = File::Spec->catfile($tmpdir, "bench.proto");
path($proto_file)->spew($proto_content);
my $descriptor_bin = `protoc -I$tmpdir --descriptor_set_out=/dev/stdout $proto_file`;
die "protoc failed" if $? != 0;

my $pool = Protobuf::DescriptorPool->generated_pool();
my $files = $pool->add_serialized_file_descriptor_set($descriptor_bin);
my $class = $files->[0]->get_top_level_message(0)->perl_class_name;

if (!$class->can('new')) {
    Protobuf::ClassGenerator->generate_for_file($files->[0]);
}

subtest 'Memoization Benchmark' => sub {
    my $msg = $class->new(id => 123, name => "memo test");
    
    note("--- Getter Performance (Existing Object) ---");
    
    # 1. Warmup / First call (Populate cache)
    my $name = $msg->name();
    
    cmpthese(-3, {
        '01_getter_cached' => sub {
            my $a = $msg->name;
        },
        '02_setter_invalidating' => sub {
            $msg->set_name("new name");
            my $a = $msg->name;
        },
    });
    
    ok(1, "Benchmarks completed");
};

done_testing();
