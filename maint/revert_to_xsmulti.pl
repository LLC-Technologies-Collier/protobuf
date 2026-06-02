use strict;
use warnings;
use Path::Tiny;

my $makefile_pl = path("Makefile.PL");
my $content = $makefile_pl->slurp_utf8;

$content =~ s/\$COMMON_LIB = 'libprotobuf_common' \. \$Config\{lib_ext\};/\$COMMON_LIB = 'libprotobuf_common\.\$(DLEXT)';/g;
$content =~ s/\\\$\(AR\) \\\$\(AR_STATIC_ARGS\) \$\{common_lib\} \$\{common_o_string\}\n\t\\\$\(RANLIB\) \$\{common_lib\}/\\\$\(CC\) -shared -o \$\{common_lib\} \$\{common_o_string\} \\\$\(LDFLAGS\)/g;

$content =~ s/XS\s*=>\s*\{.*?\},\n//g;
$content =~ s/CCFLAGS\s*=>\s*"-g \$Config\{ccflags\} -fPIC -fsanitize=address",/CCFLAGS           => "-g \$Config{ccflags} -fPIC -fsanitize=address",\n    XSMULTI           => 1,/g;

$makefile_pl->spew_utf8($content);
print "Makefile.PL modified.\n";
