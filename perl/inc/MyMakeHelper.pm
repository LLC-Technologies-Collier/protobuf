package inc::MyMakeHelper;
use strict;
use warnings;

use Config;
use File::Spec;

sub new { my $class = shift; return bless {}, $class; }

sub generate_common_lib_rules {
    my ($self, $conf) = @_;;
    my $common_o_string = $conf->{COMMON_O_STRING};
    my $common_lib = $conf->{COMMON_LIB};
    my $file_pairs = $conf->{COMMON_FILES_PAIRS};

    my $make_frag = "COMMON_LIB = $common_lib\n\n";

    my %done_o;
    my @obj_rules;
    foreach my $pair (@$file_pairs) {
        my $c_file = $pair->{c};
        my $o_file = $pair->{o};
        next if $done_o{$o_file}++;
        my $compiler = '$(CC)';
        my $bazel_gen_include = File::Spec->catfile("..", "bazel-bin", "src", "google", "protobuf", "_virtual_imports", "descriptor_proto");
        my $base_flags = "-I. -I.. -I../upb -I../src -I../third_party/utf8_range -I$bazel_gen_include";
        my $cflags = "";
        if ($c_file =~ /\.cc$/) {
            $compiler = '$(CXX)';
            $cflags = "-Wno-error=attributes -I../third_party/abseil-cpp";
        } else {
            $cflags = "-D_GNU_SOURCE -Werror=incompatible-pointer-types -Wno-error=attributes -std=c99 -Wall -Wstrict-prototypes -mssse3 -msse4.1";
        }
        push @obj_rules, "$o_file: $c_file\n" . 
                         "\t".'@mkdir -p $(@D)'."\n" .
                         "\t${compiler} ".'$(CCFLAGS) $(INC)'." ${cflags} ${base_flags} -c $c_file -o $o_file\n";
    }
    $make_frag .= join("", @obj_rules) . "\n";

    $make_frag .= <<"EOM";
# Rule to build the static library
${common_lib}: ${common_o_string} libprotobufperl.map
	\$(RM_F) ${common_lib}
	\$(CC) -shared -Wl,--version-script=libprotobufperl.map -o ${common_lib} ${common_o_string} \$(LDFLAGS)

EOM
    return $make_frag;
}

sub generate_postamble {
    my ($self, $conf) = @_;    
    my $make_frag = "";
    $make_frag .= $self->generate_common_lib_rules($conf);
    # TODO: Move other rule generation here
    return $make_frag;
}

1;
