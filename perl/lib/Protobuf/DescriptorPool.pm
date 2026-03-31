package Protobuf::DescriptorPool;

use Moo;
use strict;
use warnings;
use Carp qw(croak);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

use Protobuf::Descriptor::File;
use Protobuf::Descriptor::MessageDef;
use Protobuf::Descriptor::Enum;
use Protobuf::Descriptor::Field;
use Protobuf::ClassGenerator;

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
    my $file = _xs_add_serialized_file($self, $serialized);
    if ($file) {
        Protobuf::ClassGenerator->generate_for_file($file);
    }
    return $file;
}

sub add_serialized_file_descriptor_set {
    my ($self, $serialized) = @_;
    croak("Serialized descriptor set data is required") unless defined $serialized;
    # A FileDescriptorSet can contain multiple files, but our current XS wrapper 
    # only returns the last one parsed. Wait, the UPB API for this adds all files to the pool
    # and returns the last one. We might need a way to get all newly added files.
    # For now, generate classes for the last file. If there are dependencies, they should
    # be generated when their respective files were added during the iteration in XS.
    # Actually, upb_DefPool_AddFileSet returns the last file added.
    my $file = _xs_add_serialized_file_descriptor_set($self, $serialized);
    if ($file) {
        Protobuf::ClassGenerator->generate_for_file($file);
    }
    return $file;
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
