package Protobuf::DescriptorPool;

use Moo;
use strict;
use warnings;
use Carp qw(croak);
use Log::Any qw($log);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

use Protobuf::Descriptor::File;
use Protobuf::Descriptor::MessageDef;
use Protobuf::Descriptor::Enum;
use Protobuf::Descriptor::Field;
use Protobuf::Descriptor::OneofDef;
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

sub CLONE {
    croak("Protobuf objects cannot be safely cloned across ithreads. Use pre-forking or an event loop (e.g. Coro, AnyEvent, Mojo) instead.");
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
    my $files = _xs_add_serialized_file_descriptor_set($self, $serialized);
    if ($files && ref($files) eq 'ARRAY') {
        $log->debug("Added " . scalar(@$files) . " files from descriptor set");
        foreach my $file (@$files) {
            Protobuf::ClassGenerator->generate_for_file($file);
        }
    }
    return $files;
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
scriptors.

=cut
