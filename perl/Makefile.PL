use strict;
use warnings;
use lib 'local/lib';
use ExtUtils::MakeMaker;
use Config;
use File::Spec;
use File::Find;
use ExtUtils::Embed;
use JSON::MaybeXS;
use Template;
use Sideload::Build::FileLists qw(
    get_upb_c_files
    get_utf8_c_files
    get_generated_c_files
    get_xs_helper_c_files
);

my $asan_flags = $ENV{USE_ASAN} ? "-fsanitize=address" : "";
my $ubsan_flags = $ENV{USE_UBSAN} ? "-fsanitize=undefined" : "";
my $msan_flags = $ENV{USE_MSAN} ? "-fsanitize=memory" : "";
my $tsan_flags = $ENV{USE_TSAN} ? "-fsanitize=thread" : "";
my $sanitizer_flags = join(" ", grep { $_ } $asan_flags, $ubsan_flags, $msan_flags, $tsan_flags);

# Attempt to generate ppport.h
eval { require Devel::PPPort; };
if (!$@) {
    warn "Generating ppport.h ...\n";
    if (!Devel::PPPort::WriteFile('ppport.h')) {
        warn "Failed to generate ppport.h: $!";
    }
} else {
    warn main::_render_tt('devel_ppport_warning.mk.tt', {});
}

# Define your common C source files relative to perl/
my @UPB_C_FILES = get_upb_c_files();
my @UTF8_C_FILES = get_utf8_c_files();
my @GENERATED_C_FILES = get_generated_c_files();
my @XS_HELPER_C_FILES = get_xs_helper_c_files();

my @COMMON_C_FILES = (@UPB_C_FILES, @UTF8_C_FILES, @GENERATED_C_FILES, @XS_HELPER_C_FILES);

# Mangle object file names to avoid collisions and place them in the current dir (perl/)
my @COMMON_O_FILES = map {
    my $o = $_;
    $o =~ s{^\.\./}{}; # Remove ../
    $o =~ s/\.cc?$/$Config{obj_ext}/;
    $o;
} @COMMON_C_FILES;

my $COMMON_LIB = 'libprotobufperl.$(DLEXT)';
my $COMMON_O_STRING = join(' ', @COMMON_O_FILES);

my @COMMON_FILES_PAIRS;
for my $i (0 .. $#COMMON_C_FILES) {
    push @COMMON_FILES_PAIRS, { c => $COMMON_C_FILES[$i], o => $COMMON_O_FILES[$i] };
}

# Subroutines to override MakeMaker behavior must be defined before WriteMakefile

# Template Toolkit helper function
my $tt;
sub _render_tt {
    my ($template_file, $vars) = @_;
    if (!$tt) {
        $tt = Template->new({
            INCLUDE_PATH => [ 'inc/templates' ],
            PRE_CHOMP    => 0,
            POST_CHOMP   => 0,
            TRIM         => 0,
        });
    }
    my $output = '';
    if (!$tt->process($template_file, $vars, \$output)) {
        die "Template processing failed for $template_file: " . $tt->error();
    }
    return $output;
}


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

    my $make_frag = main::_render_tt('common_lib_def.mk.tt', { COMMON_LIB => $common_lib });
    $make_frag .= "\n";

    my %done_o;
    my @obj_rules;
    # .... OBJECT FILE RULES for libprotobufperl.a ....
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
        push @obj_rules, main::_render_tt('common_obj_rule.mk.tt', {
            c => $c_file,
            o => $o_file,
            compiler => $compiler,
            cflags => $cflags,
            base_flags => $base_flags,
        });
    }
    $make_frag .= join("", @obj_rules) . "\n";

    $make_frag .= main::_render_tt('all_common_lib_dep.mk.tt', { COMMON_LIB => $common_lib });
    $make_frag .= "\n";

    $make_frag .= main::_render_tt('common_lib_link_rule.mk.tt', {
        COMMON_LIB => $common_lib,
        COMMON_O_STRING => $common_o_string,
    });
    $make_frag .= "\n";

    # Rule for upb-perl-test.o
    $make_frag .= main::_render_tt('upb_perl_test_o.mk.tt', {});
    $make_frag .= "\n";

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
            my $compiler = ($s =~ /\.cc$/) ? '$(CXX)' : '$(CC)';
            $c_test_rules .= main::_render_tt('c_test_one_compile.mk.tt', {
                s => $s,
                o => $o,
                deps => $deps,
                compiler => $compiler,
                embed_cflags => $embed_cflags,
                test_inc => $test_inc,
                extra_ccflags => $extra_ccflags,
                space => ' ',
            });
        }

        # Rule to link the object files into an executable
        my $test_deps = "$all_objects_str \$(COMMON_LIB) t/c/upb-perl-test.o";
        if ($target eq 't/c/convert/sv_to_upb' || $target eq 't/c/convert/upb_to_sv' || $target eq 't/c/integration/050_convert') {
            $test_deps .= " t/data/test_descriptor.bin";
        }

        my $link_line = join " ", grep { length $_ > 0 } (
            '$(CC)', '$(CCFLAGS)', $extra_ccflags, '-o', $target, $all_objects_str,
            't/c/upb-perl-test.o', '-L$(CURDIR)', '-lprotobufperl',
            $embed_cflags, $embed_ldflags, $ldflags, "-lpcre2-8 $sanitizer_flags"
        );

        $c_test_rules .= main::_render_tt('c_test_one_link.mk.tt', {
            target => $target,
            test_deps => $test_deps,
            link_line => $link_line,
        });
    }
    $c_test_rules .= "# end c test rules\n";
    $make_frag .= $c_test_rules;

    # C Benchmarks rules
    my @bench_sources = glob("t/c/benchmark/*.c");
    my @c_bench_targets;
    my @benchmark_params = map {
        my $target = $_;
        $target =~ s/\.c$//;
        push @c_bench_targets, $target;
        { source => $_, target => $target, object => "$target.o" }
    } @bench_sources;

    $make_frag .= main::_render_tt('c_benchmark_rules.mk.tt', {
        benchmarks => \@benchmark_params,
        embed_cflags => $embed_cflags,
        embed_ldflags => $embed_ldflags,
        sanitizer_flags => $sanitizer_flags,
    });

    # Add all C test and benchmark targets to .PHONY
    $make_frag .= main::_render_tt('phony.mk.tt', {
        c_test_targets => \@c_test_targets,
        c_bench_targets => \@c_bench_targets,
    });

    # Rule to generate test_descriptor.bin
    $make_frag .= main::_render_tt('test_descriptor_bin.mk.tt', {});

    # Target to build all C tests & benchmarks
    $make_frag .= main::_render_tt('build_aggregator_targets.mk.tt', {
        c_test_targets  => \@c_test_targets,
        c_bench_targets => \@c_bench_targets,
    });
    $make_frag .= "\n";

    # Rules to run all C tests and benchmarks
    $make_frag .= main::_render_tt('run_targets.mk.tt', {
        c_test_targets  => \@c_test_targets,
        c_bench_targets => \@c_bench_targets,
        archlibexp      => $Config{archlibexp},
        COMMON_LIB => $common_lib,
    });
    $make_frag .= "\n";

    # Rule for TSAN test
    $make_frag .= main::_render_tt('tsan_test.mk.tt', {});
    $make_frag .= "\n";

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
    MYEXTLIB          => 'libprotobufperl.$(DLEXT)',
    CCFLAGS           => "-g $Config{ccflags} -fPIC $sanitizer_flags",
    XSMULTI           => 1,
    LIBS              => ['-L$(CURDIR) -lprotobufperl'], # Link the common shared library
    LDDLFLAGS         => "$Config{lddlflags} $sanitizer_flags",
    LDFLAGS           => "$Config{ldflags} $sanitizer_flags",
    
    # Clean the generated object files and the static library
    clean             => { FILES => join(' ', @COMMON_O_FILES, $COMMON_LIB, 'blib') },
);

package MY;

# Modify the MY::test subroutine
sub test {
    my $self = shift;
    return main::_render_tt('my_test.mk.tt', {});
}

package main;
1;
