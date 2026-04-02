package Protobuf::ClassGenerator;

use strict;
use warnings;
use Log::Any qw($log);

our %DESCRIPTOR_REGISTRY;
our %FIELD_REGISTRY;

sub generate_for_file {
    my ($class, $file) = @_;
    return unless $file;
    
    my $count = $file->top_level_message_count;
    for my $i (0 .. $count - 1) {
        my $mdef = $file->get_top_level_message($i);
        _generate_recursively($mdef);
    }
    return;
}

sub _generate_recursively {
    my ($mdef) = @_;
    _generate_for_message($mdef);
    
    my $nested_count = $mdef->nested_message_count;
    for my $i (0 .. $nested_count - 1) {
        _generate_recursively($mdef->get_nested_message($i));
    }
}

sub generate_type_library {
    my ($class, $file) = @_;
    my $pkg = $file->get_package;
    my $lib_name = $pkg;
    $lib_name =~ s/^\.//;
    $lib_name =~ s/\./::/g;
    $lib_name = "Protobuf::Types::$lib_name";

    my $code = "package $lib_name;\n";
    $code .= "use Type::Library -base;\n";
    $code .= "use Type::Utils -all;\n";
    $code .= "use Types::Standard -types;\n";
    $code .= "use Protobuf::Message;\n\n";

    my $msg_count = $file->top_level_message_count;
    for my $i (0 .. $msg_count - 1) {
        my $mdef = $file->get_top_level_message($i);
        $code .= _generate_types_recursively($mdef);
    }

    $code .= "1;\n";
    return $code;
}

sub type_library {
    my ($class, $file) = @_;
    return $class->generate_type_library($file);
}

sub _generate_types_recursively {
    my ($mdef) = @_;
    my $full_name = $mdef->full_name;
    my $normalized = $full_name;
    $normalized =~ s/^\.//;
    my $perl_class = $normalized;
    $perl_class =~ s/\./::/g;
    
    my $type_name = $mdef->name;
    
    my $code = "declare '$type_name',\n";
    $code .= "    as InstanceOf['$perl_class'],\n";
    $code .= "    where { \$_->validate };\n\n";
    
    $code .= "coerce '$type_name',\n";
    $code .= "    from HashRef, via { '$perl_class'->from_perl(\$_) };\n\n";

    my $nested_count = $mdef->nested_message_count;
    for my $i (0 .. $nested_count - 1) {
        $code .= _generate_types_recursively($mdef->get_nested_message($i));
    }
    return $code;
}

sub generate_docs {
    my ($class) = @_;
    return "<html><body><h1>Protobuf Documentation</h1></body></html>";
}

sub generate_validator_xs {
    my ($class, $mdef) = @_;
    my $full_name = $mdef->full_name;
    my $normalized = $full_name;
    $normalized =~ s/^\.//;
    my $perl_class = $normalized;
    $perl_class =~ s/\./::/g;

    my $c_func = "validate_" . $normalized;
    $c_func =~ s/[:\.]/_/g;
    
    my $code = "/* AOT Validator for $full_name */\n";
    $code .= "bool $c_func(pTHX_ SV* sv) {\n";
    $code .= "    if (!sv || !SvROK(sv)) return false;\n";
    $code .= "    if (!sv_derived_from(sv, \"$perl_class\")) return false;\n";
    
    $code .= "    HV* hv = (HV*)SvRV(sv);\n";
    
    my $field_count = $mdef->field_count;
    for my $i (0 .. $field_count - 1) {
        my $f = $mdef->get_field($i);
        if ($f->is_required) {
            my $name = $f->name;
            $code .= "    if (!hv_exists(hv, \"$name\", " . length($name) . ")) return false;\n";
        }
    }
    
    $code .= "    return true;\n}\n";
    return $code;
}

sub generate_for_message {
    my ($class, $mdef) = @_;
    return _generate_for_message($mdef);
}

sub _generate_for_message {
    my ($mdef) = @_;
    
    my $full_name = $mdef->full_name;
    my $normalized = $full_name;
    $normalized =~ s/^\.//;
    
    my $perl_class = $normalized;
    $perl_class =~ s/\./::/g;

    # Special handling for Well-Known Types
    require Protobuf::WKT;
    my $ext_class = Protobuf::WKT->get_extension_class($normalized);
    if ($ext_class) {
        eval "require $ext_class";
    }

    # Check if already generated
    if ($perl_class->can('new')) {
        _inject_wkt($perl_class, $ext_class) if $ext_class;
        return;
    }
    
    $DESCRIPTOR_REGISTRY{$perl_class} = $mdef;

    # Generate the class using string eval
    my $code = '';
    $code .= <<"EOC";
package $perl_class;
use Moo;
extends 'Protobuf::Message';
sub descriptor { return \$Protobuf::ClassGenerator::DESCRIPTOR_REGISTRY{'$perl_class'}; }
sub validate {
    my \$self = shift;
    my \$c_func = '_xs_validate_' . '$normalized';
    \$c_func =~ s/[:\\.]/_/g;
    if (\$self->can(\$c_func)) {
        return \$self->\$c_func();
    }
    return \$self->SUPER::validate();
}
EOC
    
    my $field_count = $mdef->field_count;
    for my $i (0 .. $field_count - 1) {
        my $fdef = $mdef->get_field($i);
        my $name = $fdef->name;
        $FIELD_REGISTRY{$perl_class}{$name} = $fdef;

        $code .= <<"EOC";
sub $name {
    my \$self = shift;
    if (\@_) {
        return \$self->_xs_set_by_fdef(\$Protobuf::ClassGenerator::FIELD_REGISTRY{'$perl_class'}{'$name'}, \$_[0]);
    }
    my \$val = \$self->_xs_get_by_fdef(\$Protobuf::ClassGenerator::FIELD_REGISTRY{'$perl_class'}{'$name'});
    if (ref(\$val) && ref(\$val) =~ /^Protobuf::Internal::(?:Repeated|Map)\$/) {
         my \$public_class = ref(\$val) . '::Public';
         my \$proxy;
         if (ref(\$val) eq 'Protobuf::Internal::Repeated') {
             tie \@\$proxy, 'Protobuf::Internal::Repeated', \$val;
             return bless \\@\$proxy, \$public_class;
         } else {
             tie %\$proxy, 'Protobuf::Internal::Map', \$val;
             return bless \\%\$proxy, \$public_class;
         }
    }
    return \$val;
}
sub set_$name {
    my (\$self, \$value) = \@_;
    return \$self->_xs_set_by_fdef(\$Protobuf::ClassGenerator::FIELD_REGISTRY{'$perl_class'}{'$name'}, \$value);
}
sub has_$name {
    my \$self = shift;
    return \$self->_xs_has_by_fdef(\$Protobuf::ClassGenerator::FIELD_REGISTRY{'$perl_class'}{'$name'});
}
sub clear_$name {
    my \$self = shift;
    return \$self->_xs_clear_by_fdef(\$Protobuf::ClassGenerator::FIELD_REGISTRY{'$perl_class'}{'$name'});
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
        eval $code;
    }
    die "Failed to generate class $perl_class: $@" if $@;

    if ($ext_class) {
        _inject_wkt($perl_class, $ext_class);
    }
    
    # Recursively generate nested messages
    my $nested_count = $mdef->nested_message_count;
    for my $i (0 .. $nested_count - 1) {
        _generate_for_message($mdef->get_nested_message($i));
    }
    return;
}

sub _inject_wkt {
    my ($perl_class, $ext_class) = @_;
    no strict 'refs';
    if ($ext_class->can('get_injected_methods')) {
        foreach my $method ($ext_class->get_injected_methods()) {
            my $full_sym = "${perl_class}::$method";
            *$full_sym = \&{"${ext_class}::$method"};
        }
    }
}

1;
