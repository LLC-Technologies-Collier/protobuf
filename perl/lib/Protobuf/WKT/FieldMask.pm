package Protobuf::WKT::FieldMask;

use strict;
use warnings;

sub to_string {
    my ($self) = @_;
    return join(',', @{$self->paths});
}

sub from_string {
    my ($self, $str) = @_;
    @{$self->paths} = split(',', $str);
    return $self;
}

1;
