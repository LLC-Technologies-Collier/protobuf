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
    my $dir = File::Spec->catdir($project_root, 'upb');
    return unless -d $dir;
    find(sub {
        if (/\.c$/ && !/\/test_util\//) {
            push @files, $File::Find::name;
        }
    }, $dir);
    return @files;
}

sub get_utf8_c_files {
    my ($project_root) = @_;
    my @files;
    my $dir = File::Spec->catdir($project_root, 'third_party', 'utf8_range');
    return unless -d $dir;
    # Only include utf8_range.c. Other files are either tests, 
    # stand-alone benchmarks, or included via .inc files.
    push @files, File::Spec->catfile($dir, 'utf8_range.c');
    return @files;
}

sub get_generated_c_files {
    my ($project_root) = @_;
    my @files;
    my $dir = File::Spec->catdir($project_root, 'upb', 'reflection', 'stage0');
    return unless -d $dir;
    find(sub {
        if (/\.c$/) {
            push @files, $File::Find::name;
        }
    }, $dir);
    return @files;
}

sub get_xs_helper_c_files {
    my ($project_root) = @_;
    my @files;
    my $dir = File::Spec->catdir($project_root, 'perl', 'xs', 'helpers');
    if (!-d $dir) {
        $dir = File::Spec->catdir($project_root, 'xs', 'helpers');
    }
    return unless -d $dir;
    find(sub {
        if (/\.c$/) {
            push @files, $File::Find::name;
        }
    }, $dir);
    return @files;
}

1;
