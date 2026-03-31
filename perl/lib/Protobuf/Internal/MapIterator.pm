package Protobuf::Internal::MapIterator;

use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub next_key {
    my ($self) = @_;
    return $self->_xs_next_key();
}

sub next_value {
    my ($self) = @_;
    return $self->_xs_next_value();
}

1;
