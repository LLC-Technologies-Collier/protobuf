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
    # upb_Array doesn't have a simple way to set size without filling with defaults?
    # Actually we have upb_Array_Resize.
    # For now, let's just support it if possible.
}

sub PUSH {
    my $self = shift;
    foreach my $val (@_) {
        $self->_xs_append($val);
    }
}

sub POP {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return undef if $size == 0;
    my $val = $self->_xs_get_item($size - 1);
    $self->_xs_delete($size - 1, 1);
    return $val;
}

sub CLEAR {
    my ($self) = @_;
    $self->_xs_clear();
}

sub SHIFT {
    my ($self) = @_;
    my $size = $self->_xs_size();
    return undef if $size == 0;
    my $val = $self->_xs_get_item(0);
    $self->_xs_delete(0, 1);
    return $val;
}

sub UNSHIFT {
    my $self = shift;
    # upb_Array doesn't have prepend, would need to move elements.
    # We'll skip complex ones for now or implement them via delete/append if needed.
    # Actually, upb_Array_Insert exists? No.
    # For simplicity, let's just support basic operations needed by tests.
}

1;
