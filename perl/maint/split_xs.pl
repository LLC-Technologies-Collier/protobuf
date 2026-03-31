use strict;
use warnings;
use Path::Tiny;

my $protobuf_xs = path("Protobuf.xs")->slurp_utf8;

my @sections = split /(?=# -- [A-Za-z0-9:]+ --\s*MODULE =)/, $protobuf_xs;

foreach my $section (@sections) {
    if ($section =~ /^# -- ([A-Za-z0-9:]+) --\s*MODULE = ([A-Za-z0-9:]+)\s*PACKAGE = ([A-Za-z0-9:]+)/m) {
        my $module_name = $1;
        my $package_name = $3;
        
        my $xs_file_path = "lib/" . ($package_name =~ s/::/\//gr) . ".xs";
        
        # Add headers to each XS file
        my $xs_content = <<"HEADER";
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/protobuf/arena.h"
#include "xs/protobuf/message.h"
#include "xs/message/message.h"
#include "xs/message/access.h"
#include "xs/message/serialize.h"
#include "xs/descriptor_pool/pool.h"
#include "xs/descriptor_pool/add.h"
#include "xs/descriptor_pool/find.h"
#include "xs/descriptor/file.h"
#include "xs/descriptor/message.h"
#include "xs/descriptor/enum.h"
#include "xs/descriptor/field.h"
#include "xs/descriptor/enum_value.h"
#include "xs/descriptor/oneof.h"
#include "xs/descriptor/service.h"
#include "xs/descriptor/method.h"

HEADER

        $xs_content .= $section;
        
        path($xs_file_path)->parent->mkpath;
        path($xs_file_path)->spew_utf8($xs_content);
        print "Created $xs_file_path\n";
        
        # Update .pm file
        my $pm_file_path = "lib/" . ($package_name =~ s/::/\//gr) . ".pm";
        if (-f $pm_file_path) {
            my $pm_content = path($pm_file_path)->slurp_utf8;
            $pm_content =~ s/^# Load the XS code\nuse Protobuf;$/require XSLoader;\nXSLoader::load(__PACKAGE__, \$VERSION);/gm;
            path($pm_file_path)->spew_utf8($pm_content);
            print "Updated $pm_file_path\n";
        }
    }
}
