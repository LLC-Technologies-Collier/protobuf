package Protobuf::Descriptor::FieldDef::PurePerl;

use parent 'Protobuf::Descriptor::Base::PurePerl';
use strict;
use warnings;

our $VERSION = '0.01';

sub name {
    my ($self) = @_;
    return $self->{_data}{name};
}

sub number {
    my ($self) = @_;
    return $self->{_data}{number};
}

sub type {
    my ($self) = @_;
    return $self->{_data}{type};
}

sub label {
    my ($self) = @_;
    return $self->{_data}{label};
}

sub is_repeated {
    my ($self) = @_;
    return $self->{_data}{is_repeated} || 0;
}

sub is_required {
    my ($self) = @_;
    return $self->{_data}{is_required} || 0;
}

sub is_map {
    my ($self) = @_;
    return $self->{_data}{is_map} || 0;
}

sub is_submessage {
    my ($self) = @_;
    return $self->{_data}{is_submessage} || 0;
}

sub message_type {
    my ($self) = @_;
    return $self->{_data}{message_type};
}

sub enum_type {
    my ($self) = @_;
    return $self->{_data}{enum_type};
}

1;
