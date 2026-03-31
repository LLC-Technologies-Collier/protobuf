package Protobuf::Message;

use Moo;
use strict;
use warnings;
use Carp qw(croak);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

use Protobuf::Internal::Repeated;
use Protobuf::Internal::Map;

sub new {
    my ($class) = @_;
    
    croak("Protobuf::Message->new cannot be called directly. Use a generated subclass.")
        if $class eq 'Protobuf::Message';

    my $mdef = $class->descriptor;
    croak("Class $class does not have a descriptor") unless $mdef;

    return _xs_new_from_def($mdef);
}

sub DESTROY {
    my ($self) = @_;
    _xs_free($self);
}

sub get {
    my ($self, $field_name) = @_;
    my $val = _xs_get($self, $field_name);
    
    if (ref($val) eq 'Protobuf::Internal::Repeated') {
        my @arr;
        tie @arr, 'Protobuf::Internal::Repeated', $val;
        return \@arr;
    }
    if (ref($val) eq 'Protobuf::Internal::Map') {
        my %hash;
        tie %hash, 'Protobuf::Internal::Map', $val;
        return \%hash;
    }
    
    return $val;
}

sub set {
    my ($self, $field_name, $value) = @_;
    return _xs_set($self, $field_name, $value);
}

sub has_field {
    my ($self, $field_name) = @_;
    return _xs_has($self, $field_name);
}

sub clear_field {
    my ($self, $field_name) = @_;
    return _xs_clear($self, $field_name);
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
