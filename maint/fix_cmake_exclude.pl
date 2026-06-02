use strict;
use warnings;
use Path::Tiny;

my $makefile_pl = path("Makefile.PL");
my $content = $makefile_pl->slurp_utf8;

$content =~ s/!~ m\{\/reflection\/stage0\/\}/!~ m{\/reflection\/stage0\/} \&\& \$File::Find::name !~ m{\/cmake\/}/g;

$makefile_pl->spew_utf8($content);
print "Makefile.PL patched to exclude cmake/\n";
