package TestHelpers;

use strict;
use warnings;
use Protobuf::DescriptorPool;

=head1 NAME

TestHelpers - Centralized helper functions for Protobuf Perl tests

=head1 SYNOPSIS

    use lib 't/lib';
    use TestHelpers;

    my $pool = TestHelpers->get_generated_pool();
    my $empty_pool = TestHelpers->get_empty_pool();

=cut

sub get_generated_pool {
    my $class = shift;
    return Protobuf::DescriptorPool->generated_pool();
}

sub get_empty_pool {
    my $class = shift;
    return Protobuf::DescriptorPool->new();
}

1;
