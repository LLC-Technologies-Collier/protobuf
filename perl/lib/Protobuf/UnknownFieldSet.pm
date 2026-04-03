package Protobuf::UnknownFieldSet;

use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub get_data {
    my ($self) = @_;
    return $self->_xs_get_data();
}

sub add {
    my ($self, $data) = @_;
    return $self->_xs_add($data);
}

sub clear {
    my ($self) = @_;
    return $self->_xs_clear();
}

sub delete_tag {
    my ($self, $tag) = @_;
    return $self->_xs_delete_tag($tag);
}

1;
