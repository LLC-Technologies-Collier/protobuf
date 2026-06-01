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
        if ($File::Find::name =~ m{/(lua|ruby|conformance|cmake|stage0)$}) {
            $File::Find::prune = 1;
            return;
        }
        if (/\.c$/ && $File::Find::name !~ /\/test_util\//) {
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
    my $dir;
    if (-d 'xs') {
        $dir = 'xs';
    } elsif (defined $project_root) {
        if (-d File::Spec->catdir($project_root, 'perl', 'xs')) {
            $dir = File::Spec->catdir($project_root, 'perl', 'xs');
        } elsif (-d File::Spec->catdir($project_root, 'xs')) {
            $dir = File::Spec->catdir($project_root, 'xs');
        }
    }
    return unless defined $dir && -d $dir;
    find(sub {
        if (/\.c$/) {
            push @files, $File::Find::name;
        }
    }, $dir);
    return @files;
}

1;
