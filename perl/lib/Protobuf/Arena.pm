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

sub DEMOLISH {
    my $self = shift;
    # If it was created via new_tmpfs, the XS layer handles cleanup via the blessed object.
    # If it was created via new(), it's a simple hash with _arena_ptr.
    if (exists $self->{_arena_ptr} && $self->{_arena_ptr} && !exists $self->{_is_tmpfs}) {
        _xs_destroy_raw($self->{_arena_ptr});
    }
    return;
}

sub stats {
    my $self = shift;
    return { memory_used => 0 };
}

sub Clone {
    my $self = shift;
    return Protobuf::Arena->new();
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
