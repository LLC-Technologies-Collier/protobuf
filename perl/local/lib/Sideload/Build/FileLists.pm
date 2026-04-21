package Sideload::Build::FileLists;

use strict;
use warnings;
use File::Spec;
use File::Find;
use Exporter qw(import);

our @EXPORT_OK = qw(
    get_upb_c_files
    get_utf8_c_files
    get_generated_c_files
    get_xs_helper_c_files
    get_common_c_files
);

sub get_upb_c_files {
    my ($upb_root) = @_;
    $upb_root //= File::Spec->catfile("..", "upb");
    my @files;
    find(
        sub {
            push @files, $File::Find::name
              if /\.c$/
              && $File::Find::name !~ m{/conformance/}
              && $File::Find::name !~ m{/reflection/stage0/}
              && $File::Find::name !~ m{/cmake/};
        },
        $upb_root
    );
    return @files;
}

sub get_utf8_c_files {
    my ($third_party_root) = @_;
    $third_party_root //= File::Spec->catfile("..", "third_party");
    return grep { !m{/(lemire|range)-/} }
      glob(File::Spec->catfile($third_party_root, "utf8_range", "*.c"));
}

sub get_generated_c_files {
    my ($project_root) = @_;
    $project_root //= "..";
    my $descriptor_base = File::Spec->catfile($project_root, "upb", "reflection", "stage0", "google", "protobuf");
    return (
        File::Spec->catfile($descriptor_base, "descriptor.upb.c"),
        # descriptor.upb_minitable.c is not present in stage0, and might be redundant
        # If it is needed, we might need to generate it or find it elsewhere.
        # stage0 only has descriptor.upb.c and descriptor.upb.h
    );
}

sub get_xs_helper_c_files {
    return glob("xs/*.c xs/*/*.c");
}

sub get_common_c_files {
    my %args = @_;
    return (
        get_upb_c_files($args{upb_root}),
        get_utf8_c_files($args{third_party_root}),
        get_generated_c_files($args{bazel_bin_root}),
        get_xs_helper_c_files()
    );
}

1;
