use strict;
use warnings;
use Test::More;
use File::Find;
use Path::Tiny;

my @pm_files;
find(
    sub {
        push @pm_files, $File::Find::name if /\.pm$/ && -f $_;
    },
    'blib'
);

my @moo_modules;
for my $file (@pm_files) {
    next if $file =~ m{lib/TSE/Artifact/Role/};
    my $content = path($file)->slurp_utf8();
    if ($content =~ /use\s+Moo;/ && !($content =~ /use\s+Moo::Role;/)) {
        push @moo_modules, $file;
    }
}

if( scalar @moo_modules > 0){
  plan tests => scalar @moo_modules;
}else{
  plan skip_all => 'no modules found';
}

for my $file (@moo_modules) {
    my $content = path($file)->slurp_utf8();
    like(
        $content,
        qr/__PACKAGE__->meta->make_immutable;\s+1;/,
        "$file should be made immutable"
    );
}

done_testing();
