=encoding UTF-8

=head1 NAME

Protobuf::UnknownFieldSet - Represents unknown fields in a message$/

=head1 VERSION

version 0.01$/

=head1 SYNOPSIS

    # Get the set from a message
    my $unknowns = $msg->unknown_fields;

    # Check if there are any unknown fields
    if ($unknowns->get_data) {
        print "Message has unknown fields
";
    }

    # Clear all unknown fields
    $unknowns->clear;

=head1 DESCRIPTION

When a Protocol Buffer message is parsed, fields that are not defined in the message's schema (descriptor) are considered "unknown fields". Instead of discarding them, C<upb> (and thus this module) preserves them. This allows messages to be round-tripped through systems with older or different schema versions without losing data.$/

This class provides an interface to access and manipulate the raw data of these unknown fields associated with a L<Protobuf::Message> instance.$/

=head1 METHODS

=head2 get_data()

Returns the raw binary string containing all the unknown field data, or C<undef> if there are no unknown fields.$/

=head2 add($data)

Appends the given C<$data> (raw binary string) to the unknown field set.$/

=head2 clear()

Removes all unknown fields from the message.$/

=head2 delete_tag($tag)

Deletes all unknown fields with the given C<$tag> number.$/

=head1 SEE ALSO

L<Protobuf>, L<Protobuf::Message>$/

=head1 AUTHOR

C.J. Collier <cjac@google.com>$/

=head1 COPYRIGHT AND LICENSE

This software is copyright (c) 2026 by Google LLC.$/

This is free software; you can redistribute it and/or modify it under$/
the same terms as the Perl 5 programming language system itself.$/

=cut

package Protobuf::UnknownFieldSet;

use strict;
require Protobuf;
use warnings;

our $VERSION = '0.01';


sub get_data {
    my ($self) = @_;
    return $self->_xs_get_data();
}

sub add {
    my ($self, $data) = @_;
    return $self->_xs_add($data);
}

sub clear {
    my ($self) = @_;
    return $self->_xs_clear();
}

sub delete_tag {
    my ($self, $tag) = @_;
    return $self->_xs_delete_tag($tag);
}

1;
