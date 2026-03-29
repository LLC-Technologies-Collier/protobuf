package Protobuf::DescriptorPool;

use Moo;
use strict;
use warnings;
use Carp qw(croak);

our $VERSION = '0.01';

# Load the XS code
use Protobuf;

use Protobuf::Descriptor::File;
use Protobuf::Descriptor::MessageDef;
use Protobuf::Descriptor::Enum;
use Protobuf::Descriptor::Field;

has '_pool_ptr' => (
    is       => 'ro',
    default  => sub { _xs_create_raw() },
);

sub DEMOLISH {
    my $self = shift;
    if (exists $self->{_pool_ptr} && $self->{_pool_ptr}) {
        _xs_destroy_raw($self->{_pool_ptr});
    }
}

sub generated_pool {
    return _xs_generated_pool();
}

sub add_serialized_file {
    my ($self, $serialized) = @_;
    croak("Serialized descriptor data is required") unless defined $serialized;
    return _xs_add_serialized_file($self, $serialized);
}

sub add_serialized_file_descriptor_set {
    my ($self, $serialized) = @_;
    croak("Serialized descriptor set data is required") unless defined $serialized;
    return _xs_add_serialized_file_descriptor_set($self, $serialized);
}

sub find_file_by_name {
    my ($self, $name) = @_;
    return _xs_find_file_by_name($self, $name);
}

sub find_message_by_name {
    my ($self, $name) = @_;
    return _xs_find_message_by_name($self, $name);
}

sub find_enum_by_name {
    my ($self, $name) = @_;
    return _xs_find_enum_by_name($self, $name);
}

sub find_extension_by_name {
    my ($self, $name) = @_;
    return _xs_find_extension_by_name($self, $name);
}

1;
__END__

=head1 NAME

Protobuf::DescriptorPool - Pool of Protocol Buffer descriptors

=head1 SYNOPSIS

  use Protobuf::DescriptorPool;
  my $pool = Protobuf::DescriptorPool->new;
  $pool->add_serialized_file($serialized_proto);
  my $msg_def = $pool->find_message_by_name('my.package.Message');

=head1 DESCRIPTION

This module manages a collection of Protocol Buffer descriptors.

=cut
