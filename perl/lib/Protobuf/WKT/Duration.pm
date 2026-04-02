package Protobuf::WKT::Duration;

use strict;
use warnings;

sub to_seconds {
    my ($self) = @_;
    return $self->seconds + ($self->nanos / 1_000_000_000);
}

sub from_seconds {
    my ($self, $seconds) = @_;
    my $s = int($seconds);
    my $n = int(($seconds - $s) * 1_000_000_000);
    $self->set_seconds($s);
    $self->set_nanos($n);
    return $self;
}

sub get_injected_methods {
    return qw(to_seconds from_seconds);
}

1;
