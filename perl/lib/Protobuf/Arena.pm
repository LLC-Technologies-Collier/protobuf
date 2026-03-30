package Protobuf::Arena;

use Moo;
use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

has '_arena_ptr' => (
    is       => 'ro',
    default  => sub { _xs_create_raw() },
);

sub DEMOLISH {
    my $self = shift;
    if (exists $self->{_arena_ptr} && $self->{_arena_ptr}) {
        _xs_destroy_raw($self->{_arena_ptr});
    }
}

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
