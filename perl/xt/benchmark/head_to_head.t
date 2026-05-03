use strict;
use warnings;
use Test::More;
use Benchmark qw(:all);
use Protobuf;
use Google::ProtocolBuffers;
use Google::ProtocolBuffers::Dynamic;
use lib "t/lib";
use TestHelpers;
use File::Temp qw(tempdir);
use Path::Tiny;
use File::Spec;
use Cwd;

# Use proto2 for maximum compatibility
my $proto_content = <<'EOF';
syntax = "proto2";
package bench;
message BenchMessage {
  optional int32 id = 1;
  optional string name = 2;
  repeated int32 tags = 3;
}
EOF

# 1. Setup our library
my $pool = TestHelpers->get_generated_pool();
my $tmpdir = tempdir(CLEANUP => 1);
my $proto_file = "bench.proto";
my $proto_path = File::Spec->catfile($tmpdir, $proto_file);
path($proto_path)->spew($proto_content);

my $descriptor_bin = `protoc -I$tmpdir --descriptor_set_out=/dev/stdout $proto_file`;
if ($? != 0) {
    die "protoc failed: $!";
}
$pool->add_serialized_file_descriptor_set($descriptor_bin);
my $our_class = "Bench::Bench::BenchMessage";

# 2. Setup G::PB::Dynamic
my $dynamic = Google::ProtocolBuffers::Dynamic->new;
$dynamic->load_serialized_string($descriptor_bin);
$dynamic->map_message("bench.BenchMessage", "DynamicMessage");
$dynamic->resolve_references();

# 3. Setup G::PB (Pure Perl)
my @pp_classes = Google::ProtocolBuffers->parse($proto_content,
    { create_objects => 1, create_accessors => 1 }
);
my $pp_class = $pp_classes[0];

subtest 'Head-to-Head Benchmarking' => sub {
    my $id = 12345;
    my $name = "benchmark testing string";
    my @tags = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    # warmup
    my $m_our = $our_class->new;
    $m_our->set_id($id);
    $m_our->set_name($name);
    $m_our->set_tags(\@tags);
    my $data = $m_our->serialize;

    diag("--- Accessor Performance (Existing Object) ---");
    my $m_our_a = $our_class->new;
    $m_our_a->set_id($id); $m_our_a->set_name($name);
    my $m_dyn_a = DynamicMessage->new;
    $m_dyn_a->set_id($id); $m_dyn_a->set_name($name);
    my $m_pp_a = $pp_class->new;
    $m_pp_a->{id} = $id; $m_pp_a->{name} = $name;

    cmpthese(-3, {
        '01_ours_get' => sub {
            my $a = $m_our_a->id;
            my $b = $m_our_a->name;
        },
        '02_dynamic_get' => sub {
            my $a = $m_dyn_a->get_id;
            my $b = $m_dyn_a->get_name;
        },
        '03_pp_get' => sub {
            my $a = $m_pp_a->{id};
            my $b = $m_pp_a->{name};
        },
    });

    diag("--- Setter Performance (Existing Object) ---");
    my $counter = 0;
    cmpthese(-3, {
        '01_ours_set' => sub {
            $m_our_a->set_id(++$counter);
            $m_our_a->set_name($name);
        },
        '02_dynamic_set' => sub {
            $m_dyn_a->set_id(++$counter);
            $m_dyn_a->set_name($name);
        },
        '03_pp_set' => sub {
            $m_pp_a->{id} = ++$counter;
            $m_pp_a->{name} = $name;
        },
    });

    diag("--- Serialization Performance (Bypassing Cache) ---");
    cmpthese(-3, {
        '01_ours_ser' => sub {
            $m_our_a->set_id(++$counter); # Force dirty
            my $d = $m_our_a->serialize;
        },
        '02_dynamic_ser' => sub {
            $m_dyn_a->set_id(++$counter); # Force dirty
            my $d = $m_dyn_a->encode;
        },
        '03_pp_ser' => sub {
            $m_pp_a->{id} = ++$counter;
            my $d = $m_pp_a->encode;
        },
    });

    diag("--- Parsing Performance ---");
    cmpthese(-3, {
        '01_ours_parse' => sub {
            my $m = $our_class->parse($data);
        },
        '02_dynamic_parse' => sub {
            my $m = DynamicMessage->decode($data);
        },
        '03_pp_parse' => sub {
            my $m = $pp_class->decode($data);
        },
    });

    ok(1, "Benchmarking completed");
};

done_testing();
