package Protobuf::Internal::Repeated;

use strict;
use warnings;
use Tie::Array;
our @ISA = qw(Tie::Array);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

sub TIEARRAY {
    my ($class, $xs_obj) = @_;
    return $xs_obj; # The XS object itself is the tied object
}

sub FETCH {
    my ($self, $index) = @_;
    return $self->_xs_get_item($index);
}

sub STORE {
    my ($self, $index, $value) = @_;
    return $self->_xs_set_item($index, $value);
}

sub FETCHSIZE {
    my ($self) = @_;
    return $self->_xs_size();
}

sub STORESIZE {
    my ($self, $count) = @_;
    $self->_xs_resize($count);
    return;
}

sub PUSH {
    my ($self, @values) = @_;
    foreach my $val (@values) {
        $self->_xs_append($val);
    }
    return;
}

sub POP {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return if $size == 0;
    my $val = $self->_xs_get_item($size - 1);
    $self->_xs_delete($size - 1, 1);
    return $val;
}

sub CLEAR {
    my ($self) = @_;
    $self->_xs_clear();
    return;
}

sub SHIFT {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return if $size == 0;
    my $val = $self->_xs_get_item(0);
    $self->_xs_delete(0, 1);
    return $val;
}

sub UNSHIFT {
    my ($self, @values) = @_;
    # Add elements in reverse order at index 0 to preserve ordering
    foreach my $val (reverse @values) {
        $self->_xs_insert(0, $val);
    }
    return;
}

1;
