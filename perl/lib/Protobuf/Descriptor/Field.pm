package Protobuf::Descriptor::Field;

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

sub full_name {
    my ($self) = @_;
    return _xs_full_name($self);
}

sub number {
    my ($self) = @_;
    return _xs_number($self);
}

sub type {
    my ($self) = @_;
    return _xs_type($self);
}

sub label {
    my ($self) = @_;
    return _xs_label($self);
}

sub is_repeated {
    my ($self) = @_;
    return _xs_is_repeated($self);
}

sub is_map {
    my ($self) = @_;
    return _xs_is_map($self);
}

sub is_required {
    my ($self) = @_;
    return _xs_is_required($self);
}

sub message_type {
    my ($self) = @_;
    return _xs_message_type($self);
}

sub enum_type {
    my ($self) = @_;
    return _xs_enum_type($self);
}

__PACKAGE__->meta->make_immutable;

1;
