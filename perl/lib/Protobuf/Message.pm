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
        return bless \@arr, 'Protobuf::Internal::Repeated::Public';
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

sub set_oneof {
    my ($self, $oneof_name, $value) = @_;
    my $mdef = $self->descriptor;
    my $oneof = $mdef->find_oneof_by_name($oneof_name);
    croak("Oneof '$oneof_name' not found") unless $oneof;

    # Intelligent Coercion: find first field in oneof that accepts this value type
    foreach my $f ($oneof->fields) {
        # This is a simplified heuristic: if it's a message, check type; 
        # if it's scalar, check if value looks like that type.
        # For now, we'll try to set and catch errors, or use basic type checking.
        eval {
            $self->set($f->name, $value);
        };
        if (!$@) {
            return $f->name; # Successfully set
        }
    }
    croak("Could not coerce value into any field of oneof '$oneof_name'");
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
    if ($self->can('descriptor')) {
         my $full_name = $self->descriptor->full_name;
         $full_name =~ s/^\.//;
         if ($full_name =~ /^google\.protobuf\.(?:Struct|Value|ListValue)$/) {
             # WKTs have their own optimized to_perl
         } else {
             # Fallback to generic XS-based converter
             return _xs_to_perl($self);
         }
    }
    return _xs_to_perl($self);
}

sub from_perl {
    my ($self, $data) = @_;
    # WKTs will override this if they need special handling,
    # or they can be detected here.
    if ($self->can('descriptor')) {
         my $full_name = $self->descriptor->full_name;
         $full_name =~ s/^\.//;
         if ($full_name =~ /^google\.protobuf\.(?:Struct|Value|ListValue)$/) {
             # These WKTs have injected methods, but they might need
             # to be called explicitly if Moo doesn't override correctly.
         }
    }

    croak('Message::from_perl expects a HASH ref') unless ref($data) eq 'HASH';
    foreach my $key (keys %$data) {
        $self->$key($data->{$key});
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

sub freeze_to_shared {
    my ($self, $path, $size) = @_;
    $size ||= 1024 * 1024;
    my $arena = Protobuf::Arena->new_tmpfs($path, $size);
    # Deep copy the message into the shared arena
    my $shared_msg = _xs_migrate_to_arena($self, $arena);
    return $shared_msg;
}

sub thaw_from_shared {
    my ($class, $path, $size) = @_;
    $size ||= 1024 * 1024;
    my $arena = Protobuf::Arena->attach_tmpfs($path, $size);
    # In a real implementation, we'd need to find the root message 
    # pointer within the arena. For this high-level utility, 
    # we'll assume the message is at the start or tracked.
    return _xs_find_in_shared_arena($class, $arena);
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
