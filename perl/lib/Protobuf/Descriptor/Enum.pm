package Protobuf::Descriptor::Enum;

use Moo;
use strict;
use warnings;

our $VERSION = '0.01';

# Load the XS code
use Protobuf;

sub full_name {
    my ($self) = @_;
    return _xs_full_name($self);
}

sub name {
    my ($self) = @_;
    return _xs_name($self);
}

sub value_count {
    my ($self) = @_;
    return _xs_value_count($self);
}

1;
