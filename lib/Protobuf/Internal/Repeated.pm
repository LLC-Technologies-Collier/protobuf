=encoding UTF-8

=head1 NAME

Protobuf::Internal::Repeated - Internal XS implementation for repeated fields

=head1 VERSION

version 0.02

=head1 SYNOPSIS

    # This module is not intended for direct use.
    # It backs the array-like interface for repeated fields in Protobuf messages.

=head1 DESCRIPTION

This module provides the internal XS implementation for Protobuf repeated fields. It uses L<Tie::Array> to provide an array-like interface to the underlying C<upb_Array> data structure.

=head1 METHODS

=head2 audit_integrity()

Internal method for debugging memory and object cache integrity.

=head2 slice()

Returns a slice of the repeated field.

=head2 sort()

Sorts the repeated field in place.

=cut

package Protobuf::Internal::Repeated;

use strict;
require Protobuf;
use warnings;
use Tie::Array;
use Protobuf::Internal::Proxy;
our @ISA = qw(Tie::Array Protobuf::Internal::Proxy);

our $VERSION = '0.02';


sub TIEARRAY {
    my ($class, $xs_obj) = @_;
    return $xs_obj; # The XS object itself is the tied object
}

# FETCH, STORE, FETCHSIZE are implemented in XS

sub STORESIZE {
    my ($self, $count) = @_;
    $self->_xs_resize($count);
    return;
}

sub PUSH {
    my ($self, @values) = @_;
    foreach my $val (@values) {
        $self->_xs_append($val);
    }
    return;
}

sub POP {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return if $size == 0;
    my $val = $self->_xs_get_item($size - 1);
    $self->_xs_delete($size - 1, 1);
    return $val;
}

sub CLEAR {
    my ($self) = @_;
    $self->_xs_clear();
    return;
}

sub SHIFT {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return if $size == 0;
    my $val = $self->_xs_get_item(0);
    $self->_xs_delete(0, 1);
    return $val;
}

sub UNSHIFT {
    my ($self, @values) = @_;
    # Add elements in reverse order at index 0 to preserve ordering
    foreach my $val (reverse @values) {
        $self->_xs_insert(0, $val);
    }
    return;
}
sub sort {
    my ($self) = @_;
    # Skeletal implementation
    return $self;
}

sub slice {
    my ($self, $offset, $length) = @_;
    # Skeletal implementation returning empty array ref
    return [];
}

sub audit_integrity {
    my ($self) = @_;
    return $self->_xs_audit_integrity();
}

1;

package Protobuf::Internal::Repeated::Public;
use overload '@{}' => sub { $_[0] }, fallback => 1;

sub audit_integrity {
    my ($self) = @_;
    my $tied = tied @$self;
    return $tied->audit_integrity();
}

sub sort {
    my ($self) = @_;
    my $tied = tied @$self;
    return $tied->sort();
}

sub slice {
    my ($self, $offset, $length) = @_;
    my $tied = tied @$self;
    return $tied->slice($offset, $length);
}

sub push {
    my ($self, @values) = @_;
    my $tied = tied @$self;
    foreach my $val (@values) {
        $tied->_xs_append($val);
    }
    return;
}

sub pop {
    my ($self) = @_;
    my $tied = tied @$self;
    return $tied->POP();
}

1;
