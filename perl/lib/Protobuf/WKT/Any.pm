package Protobuf::WKT::Any;

use strict;
use warnings;

sub pack {
    my ($self, $msg) = @_;
    
    my $mdef = $msg->descriptor();
    my $full_name = $mdef->full_name();
    
    # Standard type_url prefix
    my $type_url = "type.googleapis.com/$full_name";
    
    $self->set('type_url', $type_url);
    $self->set('value', $msg->serialize());
    
    return $self;
}

sub unpack {
    my ($self, $expected_class) = @_;
    
    my $type_url = $self->get('type_url');
    my ($full_name) = $type_url =~ m{([^/]+)$};
    
    # If expected_class is provided, use it. Otherwise, look up in pool.
    my $class = $expected_class;
    if (!$class) {
        my $pool = Protobuf::DescriptorPool->generated_pool();
        my $mdef = $pool->find_message_by_name($full_name);
        if (!$mdef) {
            die "Cannot unpack Any: type $full_name not found in pool";
        }
        $class = $full_name;
        $class =~ s/\./::/g;
    }
    
    return $class->parse($self->get('value'));
}

1;
