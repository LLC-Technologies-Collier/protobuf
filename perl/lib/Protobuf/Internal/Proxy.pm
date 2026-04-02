package Protobuf::Internal::Proxy;

use strict;
use warnings;

# Common delegation logic for tied objects
sub _xs_delegate {
    my ($self, $method, @args) = @_;
    my $xs = ref($self) eq 'HASH' ? $self->{_xs} : $self;
    return $xs->$method(@args);
}

1;
