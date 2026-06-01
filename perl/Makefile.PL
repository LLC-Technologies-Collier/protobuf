use strict;
use warnings;
use lib 'local/lib';
use lib 'xlib';
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

# --- Path Macros ---
use Cwd qw(abs_path getcwd);
my $cwd = getcwd();

# Determine the project root, favoring 'vendor' if it exists (for distributions)
my $raw_root = -d 'vendor' ? 'vendor' : '..';
our $project_root = File::Spec->abs2rel(abs_path($raw_root), $cwd);

our $upb_dir = File::Spec->catdir($project_root, 'upb');
our $upb_generator_dir = File::Spec->catdir($project_root, 'upb_generator');
our $src_dir = File::Spec->catdir($project_root, 'src');
our $third_party_dir = File::Spec->catdir($project_root, 'third_party');
# --- End Path Macros ---

# --- Dev Environment Automation ---
my $is_dev = (-f 'maint/generate_xs.pl' && -d '../upb');
if ($is_dev) {
    print "Development environment detected.\n";
    
    # 1. Reassemble Protobuf.xs from fragments
    print "Reassembling Protobuf.xs...\n";
    system($^X, 'maint/generate_xs.pl') == 0 or die "Failed to run generate_xs.pl: $!";
    
    # 2. Update the local vendor directory with core components
    if (-f 'maint/vendor_upb.pl') {
        print "Updating vendor/ directory...\n";
        system($^X, 'maint/vendor_upb.pl') == 0 or die "Failed to run vendor_upb.pl: $!";
    }
    
    # 3. Clean and regenerate MANIFEST from scratch to remove dead references
    require ExtUtils::Manifest;
    print "Regenerating MANIFEST...\n";
    unlink 'MANIFEST';
    ExtUtils::Manifest::mkmanifest();
}
# --- End Dev Environment Automation ---



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

sub _get_sanitizer_flags {
    my $asan_flags = $ENV{USE_ASAN} ? "-fsanitize=address" : "";
    my $ubsan_flags = $ENV{USE_UBSAN} ? "-fsanitize=undefined" : "";
    my $msan_flags = $ENV{USE_MSAN} ? "-fsanitize=memory" : "";
    my $tsan_flags = $ENV{USE_TSAN} ? "-fsanitize=thread" : "";
    return join(" ", grep { $_ } $asan_flags, $ubsan_flags, $msan_flags, $tsan_flags);
}

my $sanitizer_flags = _get_sanitizer_flags();

# Attempt to generate ppport.h
eval { require Devel::PPPort; };
if (!$@) {
    warn "Generating ppport.h ...$/";
    if (!Devel::PPPort::WriteFile('ppport.h')) {
        warn "Failed to generate ppport.h: $!";
    }
} else {
    warn main::_render_tt('devel_ppport_warning.mk.tt', {});
}

# Define your common C source files relative to perl/
my @UPB_C_FILES = get_upb_c_files($project_root);
my @UTF8_C_FILES = get_utf8_c_files($project_root);
my @GENERATED_C_FILES = get_generated_c_files($project_root);
my @XS_HELPER_C_FILES = get_xs_helper_c_files($project_root);

my @COMMON_C_FILES = (@UPB_C_FILES, @UTF8_C_FILES, @GENERATED_C_FILES, @XS_HELPER_C_FILES);

# Pure UPB object files for the protoc plugin (no Perl dependencies)
my @PURE_UPB_O_FILES = grep { $_ !~ /plugin\.upb/ } map {
    my $o = $_;
    $o =~ s{^\Q$project_root\E[\\/]}{};
    $o =~ s/\.cc?$/$Config{obj_ext}/;
    $o;
} (@UPB_C_FILES, @UTF8_C_FILES, @GENERATED_C_FILES);
my $PURE_UPB_O_STRING = join(' ', @PURE_UPB_O_FILES);

# Mangle object file names to avoid collisions and place them in the current dir (perl/)
my @COMMON_O_FILES = map {
    my $o = $_;
    $o =~ s{^\Q$project_root\E[\\/]}{}; # Remove the relative root prefix
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

sub _get_adjusted_embed_flags {
    use ExtUtils::Embed;
    use File::Spec;
    use Config;

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

    return ($embed_cflags, $embed_ldflags);
}

# Rules to build the static library and its object files
sub MY::postamble {
    my $self = shift;
    use Config;
    use File::Spec;
    use JSON::MaybeXS; 
    use Path::Tiny;

    # Common library build rules
    my $common_o_string = $COMMON_O_STRING;
    my $common_lib = $COMMON_LIB;
    my $file_pairs = \@COMMON_FILES_PAIRS;

    my $make_frag = main::_render_tt('common_lib_def.mk.tt', { COMMON_LIB => $common_lib });
    $make_frag .= $/;

    my %done_o;
    my @obj_rules;
    # .... OBJECT FILE RULES for libprotobufperl.a ....
    foreach my $pair (@$file_pairs) {
        my $c_file = $pair->{c};
        my $o_file = $pair->{o};
        next if $done_o{$o_file}++;
        my $compiler = '$(CC)';
        my $bazel_gen_include = File::Spec->catdir($upb_dir, "reflection", "stage0");
        my $cmake_gen_include = File::Spec->catdir($upb_dir, "reflection", "cmake");
        my $base_flags = "-I. -I$project_root -I$upb_dir -I$src_dir -I$third_party_dir -I" . File::Spec->catdir($third_party_dir, 'utf8_range') . " -I$bazel_gen_include -I$cmake_gen_include -DUPB_BOOTSTRAP_STAGE=0";

        my $cflags = "";
        if ($c_file =~ /\.cc$/) {
            $compiler = '$(CXX)';
            $cflags = "-Wno-error=attributes -I" . File::Spec->catdir($third_party_dir, 'abseil-cpp');
        } else {
            $cflags = "-D_GNU_SOURCE -Werror=incompatible-pointer-types -Wno-error=attributes -std=c99 -Wall -Wstrict-prototypes";
        }
        push @obj_rules, main::_render_tt('common_obj_rule.mk.tt', {
            c => $c_file,
            o => $o_file,
            compiler => $compiler,
            cflags => $cflags,
            base_flags => $base_flags,
        });
    }
    $make_frag .= join("", @obj_rules) . $/;

    $make_frag .= main::_render_tt('all_common_lib_dep.mk.tt', { COMMON_LIB => $common_lib });
    $make_frag .= $/;

    $make_frag .= main::_render_tt('common_lib_link_rule.mk.tt', {
        COMMON_LIB => $common_lib,
        COMMON_O_STRING => $common_o_string,
    });
    $make_frag .= $/;

    # Rule for upb-perl-test.o
    $make_frag .= main::_render_tt('upb_perl_test_o.mk.tt', {});
    $make_frag .= $/;

    # Custom C test rules
    my ($embed_cflags, $embed_ldflags) = main::_get_adjusted_embed_flags();
    my $archlibexp = $Config{archlibexp};

    my $config_file = File::Spec->catfile('t', 'c', 'c_test_config.json');
    my $json_text = Path::Tiny::path($config_file)->slurp_utf8;

    # Replace hardcoded monorepo paths with dynamic path macros
    $json_text =~ s{\.\./upb/}{\$upb_dir/}g;
    $json_text =~ s{\.\./src/}{\$src_dir/}g;
    $json_text =~ s{\.\./third_party/}{\$third_party_dir/}g;

    # Actually, it's better to just replace them with the actual paths 
    # because we are about to decode the JSON.
    $json_text =~ s{\$upb_dir}{$upb_dir}g;
    $json_text =~ s{\$src_dir}{$src_dir}g;
    $json_text =~ s{\$third_party_dir}{$third_party_dir}g;

    my $c_test_config = decode_json($json_text);


    my @c_test_targets;
    my $c_test_rules = "# c test rules$/";
    my $test_inc = '$(INC)'; # Inherit INC from WriteMakefile

    foreach my $test_key ( sort keys %$c_test_config ){
        next if $test_key =~ /eval_croak_test/;
        if ($test_key =~ /coro/ && $Config{archname} !~ /x86_64|i[3456]86/) {
            warn "Skipping coroutine test $test_key on non-x86 architecture ($Config{archname})$/";
            next;
        }

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
    $c_test_rules .= "# end c test rules$/";
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
    $make_frag .= main::_render_tt('test_descriptor_bin.mk.tt', {
        project_root => $project_root,
    });

    # Target to build all C tests & benchmarks
    $make_frag .= main::_render_tt('build_aggregator_targets.mk.tt', {
        c_test_targets  => \@c_test_targets,
        c_bench_targets => \@c_bench_targets,
    });
    $make_frag .= $/;

    # Rules to run all C tests and benchmarks
    $make_frag .= main::_render_tt('run_targets.mk.tt', {
        c_test_targets  => \@c_test_targets,
        c_bench_targets => \@c_bench_targets,
        archlibexp      => $Config{archlibexp},
        COMMON_LIB => $common_lib,
    });
    $make_frag .= $/;

    # Rule for TSAN test
    $make_frag .= main::_render_tt('tsan_test.mk.tt', {});
    $make_frag .= $/;

    # Rule for protoc-gen-perl-pb executable
    my $protoc_plugin_src = "protoc/protoc-gen-perl-pb.cc";
    my $protoc_plugin_obj = "protoc/protoc-gen-perl-pb.o";
    my $protoc_plugin_exe = "protoc-gen-perl-pb";
    my $plugin_upb_c = File::Spec->catfile($upb_generator_dir, "stage0", "google", "protobuf", "compiler", "plugin.upb.c");
    my $plugin_upb_o = "protoc/plugin.upb.o";
    my $descriptor_upb_c = File::Spec->catfile($upb_dir, "reflection", "stage0", "google", "protobuf", "descriptor.upb.c");
    my $descriptor_upb_o = "protoc/descriptor.upb.o";
    
    my $protoc_cflags = "-g -D_GNU_SOURCE -std=c++17 -Wall -DUPB_BOOTSTRAP_STAGE=0";
    my $protoc_inc = "-I. -I./protoc -I$project_root -I$upb_dir -I$src_dir -I$third_party_dir -I" . File::Spec->catdir($third_party_dir, 'utf8_range') . " -I" . File::Spec->catdir($upb_dir, 'reflection', 'stage0') . " -I" . File::Spec->catdir($upb_dir, 'reflection', 'cmake') . " -I" . File::Spec->catdir($upb_generator_dir, 'stage0') . " -I$Config{archlibexp}/CORE";

    $make_frag .= main::_render_tt('protoc_plugin_compile.mk.tt', {
        src => $protoc_plugin_src,
        obj => $protoc_plugin_obj,
        cflags => $protoc_cflags,
        inc => $protoc_inc,
    });
    $make_frag .= $/;

    # Rules for upb plugin objects
    $make_frag .= "$plugin_upb_o: $plugin_upb_c\n\t\$(CC) -I. -I$project_root -I$upb_dir -I" . File::Spec->catdir($upb_generator_dir, 'stage0') . " -I" . File::Spec->catdir($upb_dir, 'reflection', 'stage0') . " -O2 -c $plugin_upb_c -o $plugin_upb_o\n\n";

    $make_frag .= main::_render_tt('protoc_plugin_link.mk.tt', {
        exe => $protoc_plugin_exe,
        obj => "$protoc_plugin_obj $plugin_upb_o",
        common_lib => $COMMON_LIB,
        common_o => $PURE_UPB_O_STRING,
        sanitizer_flags => $sanitizer_flags,
    });
    $make_frag .= $/;

    # Rules to build the protoc plugin using Bazel
    $make_frag .= main::_render_tt('bazel_protoc_plugin.mk.tt', {});
    $make_frag .= $/;

    return $make_frag;

    }

    # Add bazel_build_protoc_plugin to the default dependencies
    sub MY::all_before {
    return '';
    }

# --- Clean list generation ---
my %clean_files_map = map { $_ => 1 } (
    'blib',
    'ppport.h',
    'libprotobufperl.$(DLEXT)',
    'protoc/protoc-gen-perl-pb.o',
    'protoc-gen-perl-pb',
    @COMMON_O_FILES,
    't/c/upb-perl-test.o',
);

# Add C test binaries and their objects
if (-f 't/c/c_test_config.json') {
    require JSON::MaybeXS;
    require Path::Tiny;
    my $c_test_config = JSON::MaybeXS::decode_json(Path::Tiny::path('t/c/c_test_config.json')->slurp_utf8);
    foreach my $test_key (keys %$c_test_config) {
        $clean_files_map{"t/c/$test_key"} = 1;
        $clean_files_map{"t/c/$test_key.o"} = 1;
        my $config = $c_test_config->{$test_key} || {};
        if (my $extra_src = $config->{extra_src}) {
            my @extras = ref $extra_src eq 'ARRAY' ? @$extra_src : ($extra_src);
            foreach my $src (@extras) {
                my $obj = $src;
                $obj =~ s/\.cc?$/.o/;
                $clean_files_map{$obj} = 1;
            }
        }
    }
}
# Add benchmark binaries
foreach my $t (glob("t/c/benchmark/*.c")) {
    $t =~ s/\.c$//;
    $clean_files_map{$t} = 1;
    $clean_files_map{"$t.o"} = 1;
}

my @clean_files = sort keys %clean_files_map;

WriteMakefile(
    NAME              => 'Protobuf',
    VERSION           => '0.02',
    ABSTRACT          => 'High-performance Protocol Buffers implementation using upb',
    PREREQ_PM         => {
      'Moo'             => 0,
      'Type::Tiny'      => 0,
      'Types::Standard' => 0,
      'JSON::MaybeXS'   => 0,
      'Const::Fast'     => 0,
      'Math::BigInt'    => 0,
      'MIME::Base64'    => 0,
    },
    META_MERGE => {
        'meta-spec' => { version => 2 },
        resources => {
            repository => {
                type => 'git',
                url  => 'https://github.com/LLC-Technologies-Collier/protobuf.git',
                web  => 'https://github.com/LLC-Technologies-Collier/protobuf',
            },
        },
    },
    TEST_REQUIRES => {
      'Test::More'       => 0,
      'Test::MockModule' => 0,
      'File::Spec'       => 0,
      'Path::Tiny'       => 0,
      'Test::Valgrind'   => 0, # Optional, for memory checks
      'Coro'             => 0,
    },
    CONFIGURE_REQUIRES => {
        'ExtUtils::MakeMaker' => 0,
        'Config'              => 0,
        'File::Spec'          => 0,
        'File::Find'          => 0,
        'ExtUtils::Embed'     => 0,
        'JSON::MaybeXS'       => 0,
        'Template'            => 0,
        'Path::Tiny'          => 0,
        'Cwd'                 => 0,
        'Devel::PPPort'       => 0,
    },
    AUTHOR            => 'C.J. Collier <cjac@colliertech.org>',
    LICENSE           => 'apache_2_0',
    DEFINE            => '-DUPB_BOOTSTRAP_STAGE=0',
    INC               => "-I. -I$project_root -I$upb_dir -I$src_dir -I$third_party_dir -I" . File::Spec->catdir($third_party_dir, 'utf8_range') . " -I" . File::Spec->catdir($upb_dir, 'reflection', 'stage0') . " -I" . File::Spec->catdir($upb_dir, 'reflection', 'cmake') . " -I\$(PERL_INC)",
    MYEXTLIB          => 'libprotobufperl.$(DLEXT)',
    CCFLAGS           => "-g $Config{ccflags} -fPIC $sanitizer_flags",
    LIBS              => ['-L$(CURDIR) -lprotobufperl'],
    LDDLFLAGS         => "$Config{lddlflags} $sanitizer_flags",
    LDFLAGS           => "$Config{ldflags} $sanitizer_flags",
    
    # Clean the generated object files and the static library
    clean             => { FILES => join(' ', sort @clean_files) },

    dist              => {
        PREOP => '$(PERL) maint/vendor_upb.pl && $(PERL) -MExtUtils::Manifest=mkmanifest -e "mkmanifest"',
    },
);

package MY;

# Modify the MY::test subroutine
sub test {
    my $self = shift;
    return main::_render_tt('my_test.mk.tt', {});
}

package main;
1;
