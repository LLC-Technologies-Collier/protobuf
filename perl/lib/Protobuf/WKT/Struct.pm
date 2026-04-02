package Protobuf::WKT::Struct;

use strict;
use warnings;
use JSON::MaybeXS;

sub to_perl {
    my ($self) = @_;
    my $fields = $self->fields;
    my %hash;
    foreach my $key (keys %$fields) {
        $hash{$key} = Protobuf::WKT::Value::to_perl($fields->{$key});
    }
    return \%hash;
}

sub from_perl {
    my ($self, $data) = @_;
    die 'from_perl expects a HASH ref' unless ref($data) eq 'HASH';
    my $fields = $self->fields;
    %$fields = ();
    foreach my $key (keys %$data) {
        my $val = 'google::protobuf::Value'->new();
        Protobuf::WKT::Value::from_perl($val, $data->{$key});
        $fields->{$key} = $val;
    }
    return $self;
}

sub memory_profile {
    my ($self) = @_;
    # Skeletal implementation returning dummy metrics
    return { arena_bytes => 0, field_count => 0 };
}

sub to_json {
    my ($self) = @_;
    # Skeletal implementation returning empty JSON object
    return '{}';
}

sub get_injected_methods {
    return qw(to_perl from_perl memory_profile to_json);
}

{
    package Protobuf::WKT::Value; ## no critic (Modules::ProhibitMultiplePackages)
    use strict;
    use warnings;

    sub to_perl {
        my ($self) = @_;
        my $kind = $self->kind;
        return unless defined $kind;
        if ($kind eq 'null_value') { return; }
        if ($kind eq 'number_value') { return $self->number_value; }
        if ($kind eq 'string_value') { return $self->string_value; }
        if ($kind eq 'bool_value') { return $self->bool_value; }
        if ($kind eq 'struct_value') { return Protobuf::WKT::Struct::to_perl($self->struct_value); }
        if ($kind eq 'list_value') { return Protobuf::WKT::ListValue::to_perl($self->list_value); }
        return;
    }

    sub from_perl {
        my ($self, $val) = @_;
        if (!defined $val) { $self->set_null_value(0); }
        elsif (ref($val) eq 'HASH') {
            my $s = 'google::protobuf::Struct'->new();
            Protobuf::WKT::Struct::from_perl($s, $val);
            $self->set_struct_value($s);
        }
        elsif (ref($val) eq 'ARRAY') {
            my $l = 'google::protobuf::ListValue'->new();
            Protobuf::WKT::ListValue::from_perl($l, $val);
            $self->set_list_value($l);
        }

        elsif (ref($val) eq '') {
            if (JSON::MaybeXS::is_bool($val)) {
                $self->set_bool_value($val ? 1 : 0);
            }
            elsif ($val =~ /^-?\d+(\.\d+)?$/) { $self->set_number_value($val); }
            else { $self->set_string_value($val); }
        }
        return $self;
    }

    sub get_injected_methods {
        return qw(to_perl from_perl);
    }
}

{
    package Protobuf::WKT::ListValue; ## no critic (Modules::ProhibitMultiplePackages)
    use strict;
    use warnings;

    sub to_perl {
        my ($self) = @_;
        my $values = $self->values;
        my @list;
        foreach my $v (@$values) {
            push @list, Protobuf::WKT::Value::to_perl($v);
        }
        return \@list;
    }

    sub from_perl {
        my ($self, $data) = @_;
        die 'from_perl expects an ARRAY ref' unless ref($data) eq 'ARRAY';
        my $values = $self->values;
        @$values = ();
        foreach my $v (@$data) {
            my $val = 'google::protobuf::Value'->new();
            Protobuf::WKT::Value::from_perl($val, $v);
            push @$values, $val;
        }
        return $self;
    }

    sub get_injected_methods {
        return qw(to_perl from_perl);
    }
}

1;
