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

# Our Library
use Protobuf;
use Protobuf::ClassGenerator;

# Competitors
use Google::ProtocolBuffers;
eval { require Google::ProtocolBuffers::Dynamic };

plan skip_all => "Author benchmarks" unless $ENV{AUTHOR_TESTING} || $ENV{BENCHMARK};

# --- Setup Shared Environment ---
my $tmpdir = tempdir(CLEANUP => 1);
my $proto_file = File::Spec->catfile($tmpdir, "matrix.proto");
path($proto_file)->spew(<<'EOF');
syntax = "proto3";
package bench;
message Data {
  int32 id = 1;
  string name = 2;
  repeated int32 tags = 3;
}
EOF

# 1. Setup Our Library
my $descriptor_bin = `protoc -I$tmpdir --descriptor_set_out=/dev/stdout $proto_file`;
my $pool = Protobuf::DescriptorPool->generated_pool();
$pool->add_serialized_file_descriptor_set($descriptor_bin);
my $file = $pool->find_file_by_name("matrix.proto");
my $our_class = $file->get_top_level_message(0)->perl_class_name;
Protobuf::ClassGenerator->generate_for_file($file);

# 2. Setup G::PB (Pure Perl)
Google::ProtocolBuffers->parse(<<'EOF', { create_accessors => 1 });
package pp;
message Data {
  required int32 id = 1;
  optional string name = 2;
  repeated int32 tags = 3;
}
EOF
my $pp_class = "Pp::Data";

# 3. Setup G::PB::Dynamic
my $dyn;
if ($INC{'Google/ProtocolBuffers/Dynamic.pm'}) {
    $dyn = Google::ProtocolBuffers::Dynamic->new;
    $dyn->load_string("matrix.proto", path($proto_file)->slurp);
    $dyn->map({ package => 'bench', prefix => 'dyn' });
}
my $dyn_class = "dyn::Data";

# --- Pre-calculate test data ---
my $payload_str = "A" x 1024;
my $sample_msg = $our_class->new({ id => 1, name => $payload_str, tags => [1..10] });
my $wire_data = $sample_msg->serialize();

note "--- WORKLOAD: GENERIC (Mixed Get/Set/Ser) ---";
cmpthese(-3, {
    '01_G_PB_PP' => sub {
        my $m = $pp_class->decode($wire_data);
        $m->{id}++;
        my $s = $m->{name};
        $m->encode();
    },
    '02_G_PB_DYN' => sub {
        return unless $dyn;
        my $m = $dyn_class->decode($wire_data);
        $m->set_id($m->get_id() + 1);
        my $s = $m->get_name();
        $m->encode();
    },
    '03_OURS_BAL' => sub {
        my $m = $our_class->parse($wire_data);
        $m->set_id($m->id() + 1);
        my $s = $m->name();
        $m->serialize();
    },
});

note "\n--- WORKLOAD: WRITE HEAVY (Many Mutators) ---";
cmpthese(-3, {
    '01_G_PB_PP' => sub {
        my $m = $pp_class->new();
        for (1..10) { $m->{id} = $_; $m->{name} = "val"; }
    },
    '02_G_PB_DYN' => sub {
        return unless $dyn;
        my $m = $dyn_class->new();
        for (1..10) { $m->set_id($_); $m->set_name("val"); }
    },
    '03_OURS_BAL' => sub {
        my $m = $our_class->new();
        for (1..10) { $m->set_id($_); $m->set_name("val"); }
    },
    '04_OURS_WRITE' => sub {
        my $m = $our_class->new({ profile => 'write_heavy' });
        for (1..10) { $m->set_id($_); $m->set_name("val"); }
    },
});

note "\n--- WORKLOAD: READ HEAVY (Many Getters) ---";
cmpthese(-3, {
    '01_G_PB_PP' => sub {
        my $m = $pp_class->decode($wire_data);
        for (1..100) { my $a = $m->{id}; my $b = $m->{name}; }
    },
    '02_G_PB_DYN' => sub {
        return unless $dyn;
        my $m = $dyn_class->decode($wire_data);
        for (1..100) { my $a = $m->get_id(); my $b = $m->get_name(); }
    },
    '03_OURS_BAL' => sub {
        my $m = $our_class->parse($wire_data);
        for (1..100) { my $a = $m->id(); my $b = $m->name(); }
    },
});

note "\n--- WORKLOAD: ZERO COPY (Parse Only) ---";
cmpthese(-3, {
    '01_G_PB_PP' => sub {
        my $m = $pp_class->decode($wire_data);
    },
    '02_G_PB_DYN' => sub {
        return unless $dyn;
        my $m = $dyn_class->decode($wire_data);
    },
    '03_OURS_BAL' => sub {
        my $m = $our_class->parse($wire_data);
    },
    '04_OURS_ZC' => sub {
        my $m = $our_class->parse($wire_data, { profile => 'zero_copy' });
    },
});

done_testing();
