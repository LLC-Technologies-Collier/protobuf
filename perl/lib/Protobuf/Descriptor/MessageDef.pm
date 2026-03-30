package Protobuf::Descriptor::MessageDef;

use Moo;
use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub full_name {
    my ($self) = @_;
    return _xs_full_name($self);
}

sub name {
    my ($self) = @_;
    return _xs_name($self);
}

sub field_count {
    my ($self) = @_;
    return _xs_field_count($self);
}

sub field {
    my ($self, $index) = @_;
    return _xs_field($self, $index);
}

sub find_field_by_name {
    my ($self, $name) = @_;
    return _xs_find_field_by_name($self, $name);
}

sub find_field_by_number {
    my ($self, $number) = @_;
    return _xs_find_field_by_number($self, $number);
}

1;
