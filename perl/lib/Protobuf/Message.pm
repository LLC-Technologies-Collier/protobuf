package Protobuf::Message;

use Moo;
use strict;
use warnings;
use Carp qw(croak);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub new {
    my ($class) = @_;
    
    # This is slightly tricky. For generated messages, we want them to 
    # call XS to create the underlying upb_Message.
    # But Protobuf::Message is just a base class.
    
    # If this is called on a generated class, we should have a way 
    # to get its descriptor.
    
    croak("Protobuf::Message->new cannot be called directly. Use a generated subclass.")
        if $class eq 'Protobuf::Message';

    return _xs_new_from_class($class);
}

sub DESTROY {
    my ($self) = @_;
    _xs_free($self);
}

sub get {
    my ($self, $field_name) = @_;
    return _xs_get($self, $field_name);
}

sub set {
    my ($self, $field_name, $value) = @_;
    return _xs_set($self, $field_name, $value);
}

sub serialize {
    my ($self) = @_;
    return _xs_serialize($self);
}

sub parse {
    my ($class, $data) = @_;
    return _xs_parse($class, $data);
}

1;
__END__

=head1 NAME

Protobuf::Message - Base class for Protocol Buffer messages

=head1 DESCRIPTION

This is the base class for all Protocol Buffer message objects in Perl.
It provides common methods like `get`, `set`, `serialize`, and `parse`.

=cut
