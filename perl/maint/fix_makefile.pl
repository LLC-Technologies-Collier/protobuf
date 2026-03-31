use strict;
use warnings;
use Path::Tiny;

my $makefile_pl = path("Makefile.PL");
my $content = $makefile_pl->slurp_utf8;

$content =~ s/MYEXTLIB\s*=>\s*'libprotobuf_common\$\(LIB_EXT\)',/MYEXTLIB          => 'libprotobuf_common.\$(DLEXT)',/g;
$makefile_pl->spew_utf8($content);
