#!/usr/bin/env perl

use strict;
use warnings;
use File::Spec;
use File::Path qw(make_path);
use File::Copy qw(copy);
use File::Find;
use Cwd qw(abs_path);

my $perl_dir = abs_path(File::Spec->curdir());
my $project_root = abs_path(File::Spec->catdir($perl_dir, '..'));

my $vendor_dir = File::Spec->catdir($perl_dir, 'vendor');

print "Vendoring upb and third_party into $vendor_dir...\n";

# Function to recursively copy files matching a pattern
sub recursive_copy {
    my ($src_root, $dst_root, $pattern) = @_;
    
    find({
        wanted => sub {
            return unless /$pattern$/;
            return if -d $_;
            
            my $rel_path = File::Spec->abs2rel($_, $src_root);
            my $dst_path = File::Spec->catfile($dst_root, $rel_path);
            
            my ($vol, $dir, $file) = File::Spec->splitpath($dst_path);
            make_path(File::Spec->catpath($vol, $dir, ''));
            
            copy($_, $dst_path) or die "Copy failed: $_ -> $dst_path: $!";
        },
        no_chdir => 1,
    }, $src_root);
}

# 1. Vendor upb (all headers and sources)
recursive_copy(
    File::Spec->catdir($project_root, 'upb'),
    File::Spec->catdir($vendor_dir, 'upb'),
    qr/\.(c|h|inc|upb\.c|upb\.h)$/
);

# 2. Vendor upb_generator (needed for reflection/stage0)
recursive_copy(
    File::Spec->catdir($project_root, 'upb_generator'),
    File::Spec->catdir($vendor_dir, 'upb_generator'),
    qr/\.(c|h|inc|upb\.c|upb\.h)$/
);

# 3. Vendor third_party/utf8_range
recursive_copy(
    File::Spec->catdir($project_root, 'third_party', 'utf8_range'),
    File::Spec->catdir($vendor_dir, 'third_party', 'utf8_range'),
    qr/(utf8_range\.c|utf8_range\.h|utf8_validity\.h|.*\.inc)$/
);

# 4. Vendor src/google/protobuf (protos)
recursive_copy(
    File::Spec->catdir($project_root, 'src', 'google', 'protobuf'),
    File::Spec->catdir($vendor_dir, 'src', 'google', 'protobuf'),
    qr/\.proto$/
);

print "Done.\n";
