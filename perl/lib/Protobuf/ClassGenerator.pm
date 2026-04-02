package Protobuf::ClassGenerator;

use strict;
use warnings;
use Log::Any qw($log);

our %DESCRIPTOR_REGISTRY;

sub generate_for_file {
    my ($class, $file) = @_;
    
    my $count = $file->top_level_message_count;
    for my $i (0 .. $count - 1) {
        my $mdef = $file->get_top_level_message($i);
        _generate_for_message($mdef);
    }
    return;
}

sub type_library {
    my ($class) = @_;
    # Skeletal implementation
    return "package MyProtobuf::Types; use Type::Library; 1;";
}

sub generate_docs {
    my ($class) = @_;
    # Skeletal implementation
    return "<html><body><h1>Protobuf Documentation</h1></body></html>";
}

sub _generate_for_message {
    my ($mdef) = @_;
    
    my $full_name = $mdef->full_name;
    my $normalized = $full_name;
    $normalized =~ s/^\.//;
    
    my $hex_norm = unpack('H*', $normalized);
    my $hex_target = unpack('H*', 'google.protobuf.Struct');
    $log->debug("normalized=[$normalized] hex=$hex_norm target_hex=$hex_target");

    my $perl_class = $normalized;
    $perl_class =~ s/\./::/g;

    # Special handling for Well-Known Types
    require Protobuf::WKT;
    my $wkt_logic = '';
    if (my $ext_class = Protobuf::WKT->get_extension_class($normalized)) {
        eval "require $ext_class";
        if (!$@ && $ext_class->can('get_injected_methods')) {
            foreach my $method ($ext_class->get_injected_methods()) {
                $wkt_logic .= "sub $method { shift->$ext_class\::$method(\@_) }\n";
            }
        }
    }

    # Check if already generated (non-WKT or fully-initialized WKT)
    if ($perl_class->can('new')) {
        return unless $wkt_logic;
    }
    
    $DESCRIPTOR_REGISTRY{$perl_class} = $mdef;

    # Generate the class using string eval
    my $code = '';
    if (!$perl_class->can('new')) {
        $code .= <<"EOC";
package $perl_class;
use Moo;
extends 'Protobuf::Message';
sub descriptor { return \$Protobuf::ClassGenerator::DESCRIPTOR_REGISTRY{'$perl_class'}; }
EOC
    } else {
        $code .= "package $perl_class;\n";
    }
    
    $code .= $wkt_logic;

    my $field_count = $mdef->field_count;
    for my $i (0 .. $field_count - 1) {
        my $fdef = $mdef->get_field($i);
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
        my $odef = $mdef->get_oneof($i);
        my $name = $odef->name;
        $code .= <<"EOC";
sub $name {
    my \$self = shift;
    return \$self->which_oneof('$name');
}
EOC
    }

    $code .= "1;\n";

    {
        ## no critic (BuiltinFunctions::ProhibitStringyEval)
        eval $code; ## no critic (BuiltinFunctions::ProhibitStringyEval)
    }
    die "Failed to generate class $perl_class: $@" if $@;
    
    # Recursively generate nested messages
    my $nested_count = $mdef->nested_message_count;
    for my $i (0 .. $nested_count - 1) {
        _generate_for_message($mdef->get_nested_message($i));
    }
    return;
}

1;
