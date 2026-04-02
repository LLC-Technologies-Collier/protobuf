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
    return;
}

sub CLONE {
    croak('Protobuf objects cannot be safely cloned across ithreads. Use pre-forking or an event loop (e.g. Coro, AnyEvent, Mojo) instead.');
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
        return bless \%hash, 'Protobuf::Internal::Map::Public';
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

sub which_oneof {
    my ($self, $oneof_name) = @_;
    return _xs_which_oneof($self, $oneof_name);
}

sub serialize {
    my ($self) = @_;
    return _xs_serialize($self);
}

sub to_perl {
    my ($self) = @_;
    return _xs_to_perl($self);
}

sub from_perl {
    my ($self, $data) = @_;
    croak('from_perl expects a HASH ref') unless ref($data) eq 'HASH';
    foreach my $key (keys %$data) {
        $self->set($key, $data->{$key});
    }
    return $self;
}

sub to_text {
    my ($self) = @_;
    return _xs_to_text($self);
}

sub to_text_with_unknowns {
    my ($self) = @_;
    # Skeletal implementation
    return $self->to_text();
}

sub to_wire {
    my ($self) = @_;
    # Skeletal implementation returning serialized binary
    return $self->serialize();
}

sub to_json {
    my ($self) = @_;
    return _xs_to_json($self);
}

sub to_json_streaming {
    my ($self) = @_;
    # Skeletal implementation
    return $self->to_json();
}

sub to_json_compact {
    my ($self) = @_;
    # Skeletal implementation returning JSON without whitespace
    my $json = $self->to_json();
    $json =~ s/\s+//g;
    return $json;
}

sub from_json {
    my ($class, $json_data) = @_;
    return _xs_from_json($class, $json_data);
}

sub unknown_fields {
    my ($self) = @_;
    return _xs_unknown_fields($self);
}

sub dependency_graph {
    my ($self) = @_;
    # Skeletal implementation returning self for now
    return { root => ref($self), children => [] };
}

sub perf_profile {
    my ($self) = @_;
    # Skeletal implementation returning dummy metrics
    return { serialize_time => 0, deserialize_time => 0, field_access_avg => 0 };
}

sub reset_connection {
    my ($self) = @_;
    # Skeletal implementation
    return 1;
}

sub parse {
    my ($class, $data) = @_;
    return _xs_parse($class, $data);
}

__PACKAGE__->meta->make_immutable;

1;
__END__

=head1 NAME

Protobuf::Message - Base class for Protocol Buffer messages

=head1 DESCRIPTION

This is the base class for all Protocol Buffer message objects in Perl.
It provides common methods like `get`, `set`, `serialize`, and `parse`.

=cut
