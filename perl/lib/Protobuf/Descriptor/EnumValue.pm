package Protobuf::Descriptor::EnumValue;

use Moo;
use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub name {
    my ($self) = @_;
    return _xs_name($self);
}

sub number {
    my ($self) = @_;
    return _xs_number($self);
}

sub index {
    my ($self) = @_;
    return _xs_index($self);
}

__PACKAGE__->meta->make_immutable;

1;
