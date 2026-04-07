package Sideload::Build::FileLists;

use strict;
use warnings;
warn "Loading Sideload::Build::FileLists\n";
use File::Find;
use File::Spec;
use Exporter 'import';

our @EXPORT_OK = qw(
    get_upb_c_files
    get_utf8_c_files
    get_generated_c_files
    get_xs_helper_c_files
);

sub get_upb_c_files {
    my ($project_root) = @_;
    my @files;
    find(sub {
        push @files, $File::Find::name if /\.c$/ && !/\/test_util\//;
    }, File::Spec->catdir($project_root, 'upb'));
    return @files;
}

sub get_utf8_c_files {
    my ($project_root) = @_;
    my @files;
    find(sub {
        push @files, $File::Find::name if /\.c$/;
    }, File::Spec->catdir($project_root, 'third_party', 'utf8_range'));
    return @files;
}

sub get_generated_c_files {
    my ($project_root) = @_;
    my @files;
    find(sub {
        push @files, $File::Find::name if /\.c$/;
    }, File::Spec->catdir($project_root, 'upb', 'reflection', 'stage0'));
    return @files;
}

sub get_xs_helper_c_files {
    my ($project_root) = @_;
    my @files;
    find(sub {
        push @files, $File::Find::name if /\.c$/;
    }, File::Spec->catdir($project_root, 'perl', 'xs', 'helpers'));
    return @files;
}

1;
