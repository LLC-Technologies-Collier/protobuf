package Protobuf::Descriptor::File;

use Moo;
use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load(__PACKAGE__, $VERSION);

# The object is a blessed IV (the pointer itself) from the C layer,
# but Moo can still work with it if we're careful.
# However, for simplicity, let's keep it as is.

sub name {
    my ($self) = @_;
    return _xs_name($self);
}

sub get_package {
    my ($self) = @_;
    return _xs_package($self);
}

sub top_level_message_count {
    my ($self) = @_;
    return _xs_top_level_message_count($self);
}

sub get_top_level_message {
    my ($self, $index) = @_;
    return _xs_top_level_message($self, $index);
}

__PACKAGE__->meta->make_immutable;

1;
