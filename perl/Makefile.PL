use strict;
use warnings;
use ExtUtils::MakeMaker;
use Config;
use File::Spec;
use File::Find;
use ExtUtils::Embed;
use JSON::MaybeXS;

# Attempt to generate ppport.h
eval { require Devel::PPPort; };
if (!$@) {
    warn "Generating ppport.h ...\n";
    if (!Devel::PPPort::WriteFile('ppport.h')) {
        warn "Failed to generate ppport.h: $!";
    }
} else {
    warn "######################################################################\n";
    warn "# Devel::PPPort not found. It is recommended to install it to ensure  #\n";
    warn "# maximum portability of this module.                                #\n";
    warn "# > cpan Devel::PPPort                                               #\n";
    warn "######################################################################\n";
}

# Define your common C source files relative to perl/
my @UPB_C_FILES;
find(sub {
    #push @UPB_C_FILES, $File::Find::name if $File::Find::name =~ /\.c$/ || ( $File::Find::name !~ m{/conformance/} && $File::Find::name !~ m{/reflection/stage0/} && $File::Find::name !~ m{/cmake/} );
    push @UPB_C_FILES, $File::Find::name if /\.c$/ && $File::Find::name !~ m{/conformance/} && $File::Find::name !~ m{/reflection/stage0/} && $File::Find::name !~ m{/cmake/};

}, File::Spec->catfile("..", "upb"));

my @UTF8_C_FILES = grep { !m{/(lemire|range)-/} } glob(File::Spec->catfile("..", "third_party", "utf8_range", "*.c"));
my @GENERATED_C_FILES = (
    File::Spec->catfile("..", "bazel-bin", "src", "google", "protobuf", "_virtual_imports", "descriptor_proto", "google", "protobuf", "descriptor.upb.c"),
    File::Spec->catfile("..", "bazel-bin", "src", "google", "protobuf", "_virtual_imports", "descriptor_proto", "google", "protobuf", "descriptor.upb_minitable.c"),
);

# Add our XS helper C files
my @XS_HELPER_C_FILES = glob("xs/*.c xs/*/*.c");

my @COMMON_C_FILES = (@UPB_C_FILES, @UTF8_C_FILES, @GENERATED_C_FILES, @XS_HELPER_C_FILES);

# Mangle object file names to avoid collisions and place them in the current dir (perl/)
my @COMMON_O_FILES = map {
    my $o = $_;
    $o =~ s{^\.\./}{}; # Remove ../
    $o =~ s/\.cc?$/$Config{obj_ext}/;
    $o;
} @COMMON_C_FILES;

my $COMMON_LIB = 'libprotobuf_common.$(DLEXT)';
my $COMMON_O_STRING = join(' ', @COMMON_O_FILES);

my @COMMON_FILES_PAIRS;
for my $i (0 .. $#COMMON_C_FILES) {
    push @COMMON_FILES_PAIRS, { c => $COMMON_C_FILES[$i], o => $COMMON_O_FILES[$i] };
}

# Subroutines to override MakeMaker behavior must be defined before WriteMakefile

# Rules to build the static library and its object files
sub MY::postamble {
    my $self = shift;
    use Config;
    use File::Spec;
    use JSON::MaybeXS; 

    # Common library build rules
    my $common_o_string = $COMMON_O_STRING;
    my $common_lib = $COMMON_LIB;
    my $file_pairs = \@COMMON_FILES_PAIRS;

    my $make_frag = "COMMON_LIB = $common_lib\n\n";

    my %done_o;
    my @obj_rules;
    # .... OBJECT FILE RULES for libprotobuf_common.a ....
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
# Make sure the static lib is built before the default 'all' target
all :: ${common_lib}

# Rule to build the static library
${common_lib}: ${common_o_string}
	\$(RM_F) ${common_lib}
	\$(CC) -shared -o ${common_lib} ${common_o_string} \$(LDFLAGS)

EOM

    # Rule for upb-perl-test.o
    $make_frag .= <<'EOM';
t/c/upb-perl-test.o: t/c/upb-perl-test.c t/c/upb-perl-test.h
	@mkdir -p t/c
	$(CC) $(CCFLAGS) $(INC) -I. -c t/c/upb-perl-test.c -o t/c/upb-perl-test.o

EOM

    # Custom C test rules
    use ExtUtils::Embed;
    use File::Find;
    use Path::Tiny;
    my $archlibexp = $Config{archlibexp};
    my $embed_cflags = ExtUtils::Embed::ccopts(); # CC flags for embedding
    my $embed_ldflags = ExtUtils::Embed::ldopts(); # LD flags for embedding

    # Remove potential newlines
    chomp $embed_cflags;
    chomp $embed_ldflags;

    # Replace -lperl with the actual path to libperl
    my $archlib = $Config{archlib};
    my $libperl = $Config{libperl};
    my ($lib_base) = $archlib =~ m{^(.+)/perl/};
    my $perl_lib_path = File::Spec->catfile($lib_base, $libperl);
    $embed_ldflags =~ s{-lperl}{$perl_lib_path}g;

    my $config_file = File::Spec->catfile('t', 'c', 'c_test_config.json');
    my $json_text = path($config_file)->slurp_utf8;
    my $c_test_config = decode_json($json_text);

    my @c_test_targets;
    my $c_test_rules = "# c test rules\n";
    my $test_inc = '$(INC)'; # Inherit INC from WriteMakefile

    foreach my $test_key ( sort keys %$c_test_config ){
        next if $test_key =~ /eval_croak_test/;

        my $target = "t/c/$test_key";
        my $source = "$target.c";
        my $object = "$target.o";
        push @c_test_targets, $target;

        my $config = $c_test_config->{$test_key} || {};
        my @extra_src_list;
        my $extra_src_val = $config->{extra_src};
        if (defined $extra_src_val) {
            push @extra_src_list, ref $extra_src_val eq 'ARRAY' ? @{$extra_src_val} : $extra_src_val;
        }
        my $deps = join(' ', @{$config->{deps} || []});
        my $ccflags = $config->{ccflags} || '';
        my $ldflags = $config->{ldflags} || '';
        my $extra_ccflags = $config->{extra_ccflags} || '';

        my @all_objects = ($object);
        my @all_sources = ($source);
        push @all_sources, @extra_src_list;

        foreach my $src_file (@extra_src_list) {
            my $obj_file = $src_file;
            $obj_file =~ s/\.c$/.o/;
            push @all_objects, $obj_file;
        }
        my $all_objects_str = join(" ", @all_objects);

        # Filter all_sources to avoid duplicates with COMMON_C_FILES
        my @test_sources;
        foreach my $s (@all_sources) {
            my $is_common = 0;
            foreach my $common_c (@COMMON_C_FILES) {
                if ($s eq $common_c) {
                    $is_common = 1;
                    last;
                }
            }
            push @test_sources, $s unless $is_common;
        }

        # Rule to compile all .c/.cc to .o for this test
        foreach my $s (@test_sources) {
            my $o = $s;
            $o =~ s/\.cc?$/.o/;
            next if $done_o{$o}++;
            $c_test_rules .= "$o: $s $deps t/c/upb-perl-test.h\n";
            $c_test_rules .= "\t".'@mkdir -p $(@D)'."\n";
            my $compiler = ($s =~ /\.cc$/) ? '$(CXX)' : '$(CC)';
            $c_test_rules .= "\t$compiler \$(CCFLAGS) $embed_cflags $test_inc $extra_ccflags -c $s -o $o\n\n";
        }

        # Rule to link the object files into an executable
        my $test_deps = "$all_objects_str \$(COMMON_LIB) t/c/upb-perl-test.o";
        if ($target eq 't/c/convert/sv_to_upb' || $target eq 't/c/convert/upb_to_sv' || $target eq 't/c/integration/050_convert') {
            $test_deps .= " t/data/test_descriptor.bin";
        }

        my $link_line = join " ", grep { length $_ > 0 } (
            '$(CC)', '$(CCFLAGS)', $extra_ccflags, '-o', $target, $all_objects_str,
            't/c/upb-perl-test.o', '-L$(CURDIR)', '-lprotobuf_common',
            $embed_cflags, $embed_ldflags, $ldflags, '-lpcre2-8 -fsanitize=address'
        );

        $c_test_rules .= sprintf(
            '%s: %s'. "\n\t" . '%s' . "\n\n",
            $target, $test_deps,
            $link_line
        );
    }
    $c_test_rules .= "# end c test rules\n";
    $make_frag .= $c_test_rules;

    # Add all C test targets to .PHONY
    $make_frag .= ".PHONY: " . join(" ", @c_test_targets) . " test_c build_c_tests\n";

    # Rule to generate test_descriptor.bin
    $make_frag .= <<'EOM';

t/data/test_descriptor.bin: t/c/test.proto
	@echo "Generating t/data/test_descriptor.bin from t/c/test.proto"
	@mkdir -p t/data
	protoc --descriptor_set_out=t/data/test_descriptor.bin --include_imports --proto_path=t/c --proto_path=.. t/c/test.proto
EOM

    # Target to build all C tests
    if (@c_test_targets) {
        $make_frag .= 'build_c_tests: ' . join(' ', @c_test_targets) . "\n";
    }

    # Rule to run all C tests
    if (@c_test_targets) {
        $make_frag .= 'test_c: build_c_tests $(COMMON_LIB)' . "\n";
        foreach my $target (sort @c_test_targets) {
            $make_frag .= "\t" . sprintf('@echo "Running test: %s"', $target) . "\n";
            my $run_command = 'LD_LIBRARY_PATH="$(PERL_ARCHLIB):$(LD_LIBRARY_PATH)" ASAN_OPTIONS=detect_leaks=0:abort_on_error=1 ' . $target;
            $make_frag .= "\t" . $run_command . "\n";
        }
    }

    return $make_frag;
}

WriteMakefile(
    NAME              => 'Protobuf',
    VERSION           => '0.01',
    PREREQ_PM         => {
      'Moo'             => 0,
      'Type::Tiny'      => 0,
      'Types::Standard' => 0,
      'JSON::MaybeXS'   => 0,
    },
    TEST_REQUIRES => {
      'Test::More'       => 0,
      'Test::MockModule' => 0,
      'File::Spec'       => 0,
      'Path::Tiny'       => 0,
      'Test::Valgrind'   => 0, # Optional, for memory checks
      'Coro'             => 0,
    },
    AUTHOR            => 'C.J. Collier <cjac@colliertech.org>',
    LICENSE           => 'apache_2_0',
    DEFINE            => '',
    INC               => '-I. -I.. -I../upb -I../src -I../third_party/utf8_range -I../bazel-bin/src/google/protobuf/_virtual_imports/descriptor_proto -I$(PERL_INC)',
    MYEXTLIB          => 'libprotobuf_common.$(DLEXT)',
    CCFLAGS           => "-g $Config{ccflags} -fPIC -fsanitize=address",
    XSMULTI           => 1,
    LIBS              => ['-L$(CURDIR) -lprotobuf_common'], # Link the common static library
    LDDLFLAGS         => "$Config{lddlflags} -fsanitize=address",
    
    # Link the common static library into all .so files
    # LIBS              => ['-L$(CURDIR) -lprotobuf_common'],

    # Clean the generated object files and the static library
    clean             => { FILES => join(' ', @COMMON_O_FILES, $COMMON_LIB, 'blib') },
);

package MY;

# Modify the MY::test subroutine
sub test {
    my $self = shift;
    return <<'EOM';
all :: $(COMMON_LIB)

test :: all build_c_tests test_c
	$(NOECHO) $(ECHO) "Running Perl tests"
	LD_LIBRARY_PATH="$(CURDIR):$$LD_LIBRARY_PATH" LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libasan.so.8 ASAN_OPTIONS=detect_leaks=0:abort_on_error=1 prove -b -j$$(nproc) t/*.t t/integration/*.t
EOM
}

package main;
1;
