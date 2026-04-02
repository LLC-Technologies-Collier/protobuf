package Protobuf::WKT;

use strict;
use warnings;

my %REGISTRY = (
    'google.protobuf.Any'       => 'Protobuf::WKT::Any',
    'google.protobuf.Timestamp' => 'Protobuf::WKT::Timestamp',
    'google.protobuf.Duration'  => 'Protobuf::WKT::Duration',
    'google.protobuf.Struct'    => 'Protobuf::WKT::Struct',
    'google.protobuf.Value'     => 'Protobuf::WKT::Value',
    'google.protobuf.ListValue'  => 'Protobuf::WKT::ListValue',
    'google.protobuf.FieldMask' => 'Protobuf::WKT::FieldMask',
);

sub get_extension_class {
    my ($class, $full_name) = @_;
    $full_name =~ s/^\.//;
    return $REGISTRY{$full_name};
}

sub get_all_wkts {
    return keys %REGISTRY;
}

1;
