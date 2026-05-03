use strict;
use warnings;
use Test::More;
use File::Temp qw(tempdir);
use File::Spec;
use Protobuf;

# Find protoc and our plugin
my $protoc = `which protoc`;
chomp $protoc;
plan skip_all => 'protoc not found' unless $protoc;

my $plugin = File::Spec->rel2abs('protoc-gen-perl-pb');
plan skip_all => 'plugin not found (run make build_protoc_plugin)' unless -x $plugin;

my $tmpdir = tempdir(CLEANUP => 1);
my $proto_dir = 't/protos';

subtest 'generate String::StringBytes::Bytes' => sub {
    my $out_dir = tempdir(DIR => $tmpdir, CLEANUP => 1);
    my $cmd = "$protoc --plugin=protoc-gen-perl-pb=$plugin --perl-pb_out=embed_descriptors=true:$out_dir --proto_path=$proto_dir $proto_dir/string_bytes.proto 2>&1";
    my $output = `$cmd`;
    is($?, 0, 'protoc executed successfully') or diag($output);

    my $pm_file = File::Spec->catfile($out_dir, 'String', 'StringBytes.pm');
    ok(-f $pm_file, 'Generated String/StringBytes.pm exists');

    # Load the generated module
    unshift @INC, $out_dir;
    require String::StringBytes;
    
    ok(String::StringBytes::Bytes->can('new'), 'String::StringBytes::Bytes class generated');
    my $msg = String::StringBytes::Bytes->new(v_string => 'hello');
    is($msg->v_string, 'hello', 'Message works as expected');
    
    my $bin = $msg->serialize;
    my $msg2 = String::StringBytes::Bytes->parse($bin);
    is($msg2->v_string, 'hello', 'Roundtrip works');
};

subtest 'generate ProtobufTest::Types' => sub {
    my $out_dir = tempdir(DIR => $tmpdir, CLEANUP => 1);
    my $cmd = "$protoc --plugin=protoc-gen-perl-pb=$plugin --perl-pb_out=embed_descriptors=true:$out_dir --proto_path=$proto_dir $proto_dir/types.proto 2>&1";
    my $output = `$cmd`;
    is($?, 0, 'protoc executed successfully') or diag($output);

    my $pm_file = File::Spec->catfile($out_dir, 'Protobuf', 'Types.pm');
    ok(-f $pm_file, 'Generated Protobuf/Types.pm exists');

    # Load the generated module
    unshift @INC, $out_dir;
    require Protobuf::Types;
    
    ok(Protobuf::Types::Types->can('new'), 'Protobuf::Types::Types class generated');
    my $msg = Protobuf::Types::Types->new(opt_int32 => 123);
    is($msg->opt_int32, 123, 'Scalar field works');
    
    ok(Protobuf::Types::Types::Enum->can('value1'), 'Nested enum generated');
    is(Protobuf::Types::Types::Enum->value1(), 1, 'Enum value matches');
};

done_testing;
