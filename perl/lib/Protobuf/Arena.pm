package Protobuf::Arena;

use Moo;
use strict;
use warnings;

use Protobuf::Internal; # Ensure audit logging is available
use Carp qw(croak);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

has '_arena_ptr' => (
    is       => 'ro',
    default  => sub { _xs_create_raw() },
);

sub new_tmpfs {
    my ($class, $path, $size) = @_;
    croak("Usage: $class->new_tmpfs(path, size)") unless defined $path && defined $size;
    return _xs_create_tmpfs_raw($path, $size);
}

sub attach_tmpfs {
    my ($class, $path, $size) = @_;
    croak("Usage: $class->attach_tmpfs(path, size)") unless defined $path && defined $size;
    return _xs_attach_tmpfs_raw($path, $size);
}

sub set_numa_node {
    my ($self, $node) = @_;
    return $self->_xs_set_numa_node($node);
}

sub DEMOLISH {
    my $self = shift;
    $self->_xs_destroy();
    return;
}

sub Clone {
    my $self = shift;
    return Protobuf::Arena->new();
}

sub __test_canary_corruption {
    my $self = shift;
    $self->_xs_test_canary_corruption();
    return;
}

sub __test_block_canary {
    my $self = shift;
    $self->_xs_test_block_canary();
    return;
}

__PACKAGE__->meta->make_immutable;

1;
__END__

=head1 NAME

Protobuf::Arena - Memory arena for Protocol Buffer messages

=head1 SYNOPSIS

  use Protobuf::Arena;
  my $arena = Protobuf::Arena->new;

=head1 DESCRIPTION

This module wraps the `upb_Arena` C type, providing an efficient way to
manage memory for Protocol Buffer messages.

=cut
