package inc::MyMakeHelper;
use strict;
use warnings;

sub new { my $class = shift; return bless {}, $class; }

sub generate_postamble {
    my ($self, $conf) = @_;    
    my $make_frag = "";
    # TODO: Move rule generation here
    return $make_frag;
}

1;
