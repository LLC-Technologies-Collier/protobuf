#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;

my @files = glob("t/c/convert/types/*.c");

foreach my $file (@files) {
    open my $fh, '<', $file or die "Cannot open $file: $!";
    my $content = do { local $/; <$fh> };
    close $fh;

    if ($content =~ /check_sv_repeated_/) {
        # Add the include if it's missing
        if ($content !~ m|#include "xs/repeated/repeated.h"|) {
            $content =~ s|(#include <stdint.h>)|$1\n#include "xs/repeated/repeated.h"|;
            $content =~ s|(#include "upb/message/message.h")|$1\n#include "xs/repeated/repeated.h"| unless $content =~ /stdint\.h/;
        }

        # Find the type-specific name and check macro
        my ($base_type) = basename($file) =~ /^(.*)\.c$/;

        # Replace the entire check_sv_repeated function body
        $content =~ s/static void check_sv_repeated_(\w+)_val\(pTHX_ SV \*sv, const char \*prefix\) \{(.*?)\n\}/replace_func($1, $2)/seog;

        open my $out, '>', $file or die "Cannot open $file for writing: $!";
        print $out $content;
        close $out;
        print "Fixed $file\n";
    }
}

sub replace_func {
    my ($type, $body) = @_;
    
    # We will rewrite the function to check sv_derived_from instead of SvTYPE == SVt_PVAV
    my $new_body = <<"END";
static void check_sv_repeated_${type}_val(pTHX_ SV *sv, const char *prefix) {
    ok(sv_derived_from(sv, "Protobuf::Internal::Repeated"), sdiagnostic("%s: SV is a Repeated wrapper", prefix));
    if (!sv_derived_from(sv, "Protobuf::Internal::Repeated")) return;

    int size = PerlUpb_Repeated_Size(aTHX_ sv);
END

    # Add size check based on the type
    if ($type eq 'message' || $type eq 'group') {
        $new_body .= <<"END";
    is(size, 1, sdiagnostic("%s: Array has 1 element", prefix));
    SV *elem0 = PerlUpb_Repeated_GetItem(aTHX_ sv, 0);
    ok(elem0, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem0) {
        ok(SvROK(elem0), sdiagnostic("%s: Element 0 is a reference", prefix));
        SvREFCNT_dec(elem0);
    }
END
    } else {
        $new_body .= <<"END";
    is(size, 2, sdiagnostic("%s: Array has 2 elements", prefix));
    SV *elem0 = PerlUpb_Repeated_GetItem(aTHX_ sv, 0);
    ok(elem0, sdiagnostic("%s: Fetched element 0", prefix));
    if (elem0) {
        ok(SvPOK(elem0) || SvIOK(elem0) || SvNOK(elem0), sdiagnostic("%s: Element 0 is valid", prefix));
        SvREFCNT_dec(elem0);
    }
    SV *elem1 = PerlUpb_Repeated_GetItem(aTHX_ sv, 1);
    ok(elem1, sdiagnostic("%s: Fetched element 1", prefix));
    if (elem1) {
        ok(SvPOK(elem1) || SvIOK(elem1) || SvNOK(elem1), sdiagnostic("%s: Element 1 is valid", prefix));
        SvREFCNT_dec(elem1);
    }
END
    }
    
    $new_body .= "}";
    return $new_body;
}

