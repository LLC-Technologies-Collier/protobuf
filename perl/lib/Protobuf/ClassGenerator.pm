package Protobuf::ClassGenerator;

use strict;
use warnings;

sub generate_for_file {
    my ($class, $file) = @_;
    
    my $count = $file->top_level_message_count;
    for my $i (0 .. $count - 1) {
        my $mdef = $file->top_level_message($i);
        _generate_for_message($mdef);
    }
}

sub _generate_for_message {
    my ($mdef) = @_;
    
    my $full_name = $mdef->full_name;
    my $normalized = $full_name;
    $normalized =~ s/^\.//;
    
    my $hex_norm = unpack("H*", $normalized);
    my $hex_target = unpack("H*", "google.protobuf.Struct");
    warn "DEBUG: normalized=[$normalized] hex=$hex_norm target_hex=$hex_target";

    my $perl_class = $normalized;
    $perl_class =~ s/\./::/g;
    
    # Special handling for Well-Known Types
    my %wkt_map = (
        'google.protobuf.Any' => 'Any',
        'google.protobuf.Timestamp' => 'Timestamp',
        'google.protobuf.Duration' => 'Duration',
        'google.protobuf.Struct' => 'Struct',
        'google.protobuf.Value' => 'Value',
        'google.protobuf.ListValue' => 'ListValue',
        'google.protobuf.FieldMask' => 'FieldMask',
    );

    my $wkt_logic = "";
    if (my $type = $wkt_map{$normalized} || ($normalized =~ /Struct$/ ? 'Struct' : undef)) {
        warn "DEBUG: MATCHED WKT $type for $normalized";
        if ($type eq 'Any' && !$perl_class->can('pack')) {
            require Protobuf::WKT::Any;
            $wkt_logic = "sub pack { shift->Protobuf::WKT::Any::pack(\@_) } sub unpack { shift->Protobuf::WKT::Any::unpack(\@_) }\n";
        }
        elsif ($type eq 'Timestamp' && !$perl_class->can('to_time_piece')) {
            require Protobuf::WKT::Timestamp;
            $wkt_logic = "sub to_time_piece { shift->Protobuf::WKT::Timestamp::to_time_piece(\@_) } sub from_time_piece { shift->Protobuf::WKT::Timestamp::from_time_piece(\@_) } sub to_iso8601 { shift->Protobuf::WKT::Timestamp::to_iso8601(\@_) }\n";
        }
        elsif ($type eq 'Duration' && !$perl_class->can('to_seconds')) {
            require Protobuf::WKT::Duration;
            $wkt_logic = "sub to_seconds { shift->Protobuf::WKT::Duration::to_seconds(\@_) } sub from_seconds { shift->Protobuf::WKT::Duration::from_seconds(\@_) }\n";
        }
        elsif ($type eq 'Struct' && !$perl_class->can('to_perl')) {
            require Protobuf::WKT::Struct;
            $wkt_logic = "sub to_perl { shift->Protobuf::WKT::Struct::to_perl(\@_) } sub from_perl { shift->Protobuf::WKT::Struct::from_perl(\@_) }\n";
        }
        elsif ($type eq 'Value' && !$perl_class->can('to_perl')) {
            require Protobuf::WKT::Struct;
            $wkt_logic = "sub to_perl { shift->Protobuf::WKT::Value::to_perl(\@_) } sub from_perl { shift->Protobuf::WKT::Value::from_perl(\@_) }\n";
        }
        elsif ($type eq 'ListValue' && !$perl_class->can('to_perl')) {
            require Protobuf::WKT::Struct;
            $wkt_logic = "sub to_perl { shift->Protobuf::WKT::ListValue::to_perl(\@_) } sub from_perl { shift->Protobuf::WKT::ListValue::from_perl(\@_) }\n";
        }
        elsif ($type eq 'FieldMask' && !$perl_class->can('to_string')) {
            require Protobuf::WKT::FieldMask;
            $wkt_logic = "sub to_string { shift->Protobuf::WKT::FieldMask::to_string(\@_) } sub from_string { shift->Protobuf::WKT::FieldMask::from_string(\@_) }\n";
        }
    }

    # Check if already generated (non-WKT or fully-initialized WKT)
    if ($perl_class->can('new')) {
        return unless $wkt_logic;
    }
    
    # Generate the class using string eval
    my $code = "";
    if (!$perl_class->can('new')) {
        $code .= <<"EOC";
package $perl_class;
use Moo;
extends 'Protobuf::Message';
sub descriptor { return \$mdef; }
EOC
    } else {
        $code .= "package $perl_class;\n";
    }
    
    $code .= $wkt_logic;

    my $field_count = $mdef->field_count;
    for my $i (0 .. $field_count - 1) {
        my $fdef = $mdef->field($i);
        my $name = $fdef->name;
        $code .= <<"EOC";
sub $name {
    my \$self = shift;
    return \$self->get('$name');
}
sub set_$name {
    my (\$self, \$value) = \@_;
    return \$self->set('$name', \$value);
}
sub has_$name {
    my \$self = shift;
    return \$self->has_field('$name');
}
sub clear_$name {
    my \$self = shift;
    return \$self->clear_field('$name');
}
EOC
    }

    my $oneof_count = $mdef->oneof_count;
    for my $i (0 .. $oneof_count - 1) {
        my $odef = $mdef->oneof($i);
        my $name = $odef->name;
        $code .= <<"EOC";
sub $name {
    my \$self = shift;
    return \$self->which_oneof('$name');
}
EOC
    }

    $code .= "1;\n";

    eval $code;
    die "Failed to generate class $perl_class: $@" if $@;
    
    # Recursively generate nested messages
    my $nested_count = $mdef->nested_message_count;
    for my $i (0 .. $nested_count - 1) {
        _generate_for_message($mdef->nested_message($i));
    }
}

1;
