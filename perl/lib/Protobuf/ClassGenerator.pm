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
    my $perl_class = $full_name;
    $perl_class =~ s/\./::/g;
    
    # Check if already generated
    return if $perl_class->can('new');
    
    # Generate the class using string eval
    my $code = <<"EOC";
package $perl_class;
use Moo;
extends 'Protobuf::Message';
sub descriptor { return \$mdef; }
EOC

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
