#!/usr/bin/env perl

use strict;
use warnings;
use File::Spec;
use Cwd 'abs_path';
use Path::Tiny qw(path);
use File::Find;
use JSON::MaybeXS;
use Config;
use Template;
use lib 'local/lib';
use Sideload::Build::FileLists qw(
    get_upb_c_files
    get_utf8_c_files
    get_generated_c_files
    get_xs_helper_c_files
);

my $perl_dir = abs_path(File::Spec->curdir());
my $project_root = abs_path(File::Spec->catdir($perl_dir, '..'));

my $output_file = File::Spec->catfile($perl_dir, 'BUILD.bazel');

my @xs_helper_c_files = get_xs_helper_c_files();

my @common_c_files = (@xs_helper_c_files);
@common_c_files = map { s{^\Q$project_root/\E}{}; $_ } @common_c_files;

my $embed_cflags = `perl -MExtUtils::Embed -e 'print ccopts()'`;
chomp $embed_cflags;
my @perl_ccopts = split /\s+/, $embed_cflags;

my $embed_ldflags = `perl -MExtUtils::Embed -e 'print ldopts()'`;
chomp $embed_ldflags;
my @perl_linkopts = split /\s+/, $embed_ldflags;

my $config_file = File::Spec->catfile($perl_dir, 't', 'c', 'c_test_config.json');
my $json_text = path($config_file)->slurp_utf8;
my $c_test_config = decode_json($json_text);

my @c_tests;
my @c_test_targets;
foreach my $test_key ( sort keys %$c_test_config ) {
    next if $test_key =~ /eval_croak_test/;
    my $target_name = 'c_test_' . $test_key;
    $target_name =~ s{/}{-}g;
    push @c_test_targets, ':' . $target_name;

    my $config = $c_test_config->{$test_key};
    my @srcs = ('t/c/' . $test_key . '.c');
    if (exists $config->{extra_src}) {
        my $extra_src = $config->{extra_src};
        if (ref $extra_src eq 'ARRAY') {
            push @srcs, @{$extra_src};
        } else {
            push @srcs, $extra_src;
        }
    }

    my @deps = (':libprotobufperl', ':upb_perl_test_lib'); # , '@pcre2//:pcre2');
    if (exists $config->{deps} && ref $config->{deps} eq 'ARRAY') {
        push @deps, grep { $_ !~ /\.h$/ } map { my $d = $_; $d =~ s{^\.\./}{}; $d } @{$config->{deps}};
    }
    my %seen;
    @deps = grep { !$seen{$_}++ } @deps;

    push @c_tests, {
        target_name => $target_name,
        srcs => \@srcs,
        ccflags => $config->{ccflags} || '',
        extra_ccflags => $config->{extra_ccflags} || '',
        ldflags => $config->{ldflags} || '',
        deps => \@deps,
        core_path => File::Spec->catfile($Config{archlibexp}, 'CORE'),
        perl_ccopts => \@perl_ccopts,
        perl_linkopts => \@perl_linkopts,
    };
}

my @bench_sources = glob 't/c/benchmark/*.c';
my @c_benchmarks;
foreach my $src (@bench_sources) {
    my $target_name = $src;
    $target_name =~ s{t/c/benchmark/(.*)\.c$}{c_bench_$1};
    push @c_benchmarks, {
        target_name => $target_name,
        src => $src,
        core_path => File::Spec->catfile($Config{archlibexp}, 'CORE'),
        perl_linkopts => \@perl_linkopts,
    };
}

my $vars = {
    project_root    => $project_root,
    common_c_files  => \@common_c_files,
    Config          => \%Config,
    USE_ASAN        => $ENV{USE_ASAN},
    USE_UBSAN       => $ENV{USE_UBSAN},
    USE_MSAN        => $ENV{USE_MSAN},
    USE_TSAN        => $ENV{USE_TSAN},
    c_tests         => \@c_tests,
    c_benchmarks    => \@c_benchmarks,
    c_test_targets  => \@c_test_targets,
};

my $tt = Template->new({
    INCLUDE_PATH => [ File::Spec->catdir($perl_dir, 'inc', 'templates') ],
    PRE_CHOMP    => 0,
    POST_CHOMP   => 0,
    TRIM         => 0,
    TTC_DIR      => undef,
});

my $content = '';
if (!$tt->process('BUILD.bazel.tt', $vars, \$content)) {
    die 'Template processing failed: ' . $tt->error();
}

path($output_file)->spew_utf8($content);
print "Successfully wrote to $output_file$/";

exit 0;
