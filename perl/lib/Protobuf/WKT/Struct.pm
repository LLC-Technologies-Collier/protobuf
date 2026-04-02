package Protobuf::WKT::Struct;

use strict;
use warnings;

sub to_perl {
    my ($self) = @_;
    my $fields = $self->fields;
    my %hash;
    foreach my $key (keys %$fields) {
        $hash{$key} = $fields->{$key}->to_perl();
    }
    return \%hash;
}

sub from_perl {
    my ($self, $data) = @_;
    die 'from_perl expects a HASH ref' unless ref($data) eq 'HASH';
    my $fields = $self->fields;
    %$fields = ();
    foreach my $key (keys %$data) {
        my $val = google::protobuf::Value->new();
        $val->from_perl($data->{$key});
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
        if ($kind eq 'struct_value') { return $self->struct_value->to_perl(); }
        if ($kind eq 'list_value') { return $self->list_value->to_perl(); }
        return;
    }

    sub from_perl {
        my ($self, $val) = @_;
        if (!defined $val) { $self->set_null_value(0); }
        elsif (ref($val) eq 'HASH') {
            my $s = google::protobuf::Struct->new();
            $s->from_perl($val);
            $self->set_struct_value($s);
        }
        elsif (ref($val) eq 'ARRAY') {
            my $l = google::protobuf::ListValue->new();
            $l->from_perl($val);
            $self->set_list_value($l);
        }
        elsif (ref($val) eq '') {
            # Check if it looks like a number
            if ($val =~ /^-?\d+(\.\d+)?$/) { $self->set_number_value($val); }
            else { $self->set_string_value($val); }
        }
        return $self;
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
            push @list, $v->to_perl();
        }
        return \@list;
    }

    sub from_perl {
        my ($self, $data) = @_;
        die 'from_perl expects an ARRAY ref' unless ref($data) eq 'ARRAY';
        my $values = $self->values;
        @$values = ();
        foreach my $v (@$data) {
            my $val = google::protobuf::Value->new();
            $val->from_perl($v);
            push @$values, $val;
        }
        return $self;
    }
}

1;
