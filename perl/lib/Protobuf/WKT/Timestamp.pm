package Protobuf::WKT::Timestamp;

use strict;
use warnings;
use Time::Piece;

sub to_time_piece {
    my ($self) = @_;
    my $seconds = $self->seconds;
    return Time::Piece->new($seconds);
}

sub from_time_piece {
    my ($self, $tp) = @_;
    $self->set_seconds($tp->epoch);
    $self->set_nanos(0); # Time::Piece doesn't support nanos well
    return $self;
}

sub to_iso8601 {
    my ($self) = @_;
    my $tp = $self->to_time_piece();
    return $tp->datetime . 'Z';
}

sub get_injected_methods {
    return qw(to_time_piece from_time_piece to_iso8601);
}

1;
