=encoding UTF-8

=head1 NAME

Protobuf::Internal::Map - Internal XS implementation for map fields$/

=head1 VERSION

version 0.01$/

=head1 SYNOPSIS

    # This module is not intended for direct use.
    # It backs the hash-like interface for map fields in Protobuf messages.

=head1 DESCRIPTION

This module provides the internal XS implementation for Protobuf map fields. It uses L<Tie::Hash> to provide a hash-like interface to the underlying C<upb_Map> data structure.$/

=cut

package Protobuf::Internal::Map;

use strict;
require Protobuf;
use warnings;

our $VERSION = '0.01';


use Protobuf::Internal::MapIterator;

sub TIEHASH {
    my ($class, $xs_obj) = @_;
    return $xs_obj;
}

# FETCH, STORE, DELETE, EXISTS, CLEAR, SCALAR are implemented in XS

sub FIRSTKEY {
    my ($self) = @_;
    my $iter = $self->_xs_new_iterator();
    $self->{_iter} = $iter;
    return $iter->next_key();
}

sub NEXTKEY {
    my ($self, $lastkey) = @_;
    my $iter = $self->{_iter};
    return unless $iter;
    my $key = $iter->next_key();
    delete $self->{_iter} unless defined $key;
    return $key;
}

sub as_hash {
    my ($self) = @_;
    my %hash;
    my $iter = $self->_xs_new_iterator();
    while (defined(my $key = $iter->next_key())) {
        $hash{$key} = $self->FETCH($key);
    }
    return \%hash;
}

sub copy_from {
    my ($self, $other) = @_;
    # Clear then copy all from other
    $self->CLEAR();
    my $h = $other->as_hash();
    foreach my $k (keys %$h) {
        $self->STORE($k, $h->{$k});
    }
    return;
}

package Protobuf::Internal::Map::Public;

sub as_hash {
    my ($self) = @_;
    my $tied = tied %$self;
    return $tied->as_hash();
}

sub copy_from {
    my ($self, $other) = @_;
    my $tied = tied %$self;
    my $other_tied = (ref($other) eq 'HASH') ? $other : (tied %$other);
    return $tied->copy_from($other_tied);
}

1;
