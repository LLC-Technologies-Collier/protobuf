package Protobuf::Descriptor::OneofDef;

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

sub get_field {
    my ($self, $index) = @_;
    return _xs_field($self, $index);
}

sub fields {
    my ($self) = @_;
    my @fields;
    for (my $i = 0; $i < $self->field_count; $i++) {
        push @fields, $self->get_field($i);
    }
    return @fields;
}

sub is_synthetic {
    my ($self) = @_;
    return _xs_is_synthetic($self);
}

__PACKAGE__->meta->make_immutable;

1;
