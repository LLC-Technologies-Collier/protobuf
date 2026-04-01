package Protobuf::Internal::Map;

use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

use Protobuf::Internal::MapIterator;

sub TIEHASH {
    my ($class, $xs_obj) = @_;
    return bless { _xs => $xs_obj }, 'Protobuf::Internal::Map::Tied';
}

package Protobuf::Internal::Map::Tied;
use Tie::Hash;
our @ISA = qw(Tie::Hash);

sub FETCH {
    my ($self, $key) = @_;
    return $self->{_xs}->_xs_get_item($key);
}

sub STORE {
    my ($self, $key, $value) = @_;
    return $self->{_xs}->_xs_set_item($key, $value);
}

sub DELETE {
    my ($self, $key) = @_;
    my $val = $self->FETCH($key);
    $self->{_xs}->_xs_delete_item($key);
    return $val;
}

sub CLEAR {
    my ($self) = @_;
    $self->{_xs}->_xs_clear();
}

sub EXISTS {
    my ($self, $key) = @_;
    return defined $self->FETCH($key); # Good enough for now
}

sub FIRSTKEY {
    my ($self) = @_;
    my $iter = $self->{_xs}->_xs_new_iterator();
    $self->{_iter} = $iter;
    return $iter->next_key();
}

sub NEXTKEY {
    my ($self, $lastkey) = @_;
    my $iter = $self->{_iter};
    return undef unless $iter;
    my $key = $iter->next_key();
    delete $self->{_iter} unless defined $key;
    return $key;
}

sub SCALAR {
    my ($self) = @_;
    return $self->{_xs}->_xs_size();
}

1;
