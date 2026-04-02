package Protobuf::WKT::Struct;

use strict;
use warnings;
use JSON::MaybeXS;
use Scalar::Util qw(reftype);

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
    if (!ref($self)) { $self = $self->new(); }
    my $rt = reftype($data) || '';
    die 'Struct::from_perl expects a HASH ref' unless $rt eq 'HASH';
    my $fields = $self->fields;
    %$fields = ();
    foreach my $key (keys %$data) {
        my $val = 'google::protobuf::Value'->new();
        $val->from_perl($data->{$key});
        $fields->{$key} = $val;
    }
    return $self;
}

sub memory_profile {
    return { arena_bytes => 0, field_count => 0 };
}

sub to_json {
    my ($self) = @_;
    return JSON::MaybeXS->new(utf8 => 1)->encode($self->to_perl);
}

sub get_injected_methods {
    return qw(to_perl from_perl memory_profile to_json);
}

{
    package Protobuf::WKT::Value; ## no critic (Modules::ProhibitMultiplePackages)
    use strict;
    use warnings;
    use Scalar::Util qw(reftype);

    sub to_perl {
        my ($self) = @_;
        my $kind = $self->which_oneof('kind');
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
        if (!ref($self)) { $self = $self->new(); }
        my $rt = reftype($val) || '';
        if (!defined $val) { $self->null_value(0); }
        elsif ($rt eq 'HASH') {
            my $s = 'google::protobuf::Struct'->new();
            $s->from_perl($val);
            $self->struct_value($s);
        }
        elsif ($rt eq 'ARRAY') {
            my $l = 'google::protobuf::ListValue'->new();
            $l->from_perl($val);
            $self->list_value($l);
        }
        elsif ($rt eq '') {
            if (JSON::MaybeXS::is_bool($val)) {
                $self->bool_value($val ? 1 : 0);
            }
            elsif ($val =~ /^-?\d+(\.\d+)?$/) { $self->number_value($val); }
            else { $self->string_value($val); }
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
    use Scalar::Util qw(reftype);

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
        if (!ref($self)) { $self = $self->new(); }
        my $rt = reftype($data) || '';
        die 'ListValue::from_perl expects an ARRAY ref' unless $rt eq 'ARRAY';
        my $values = $self->values;
        @$values = ();
        foreach my $v (@$data) {
            my $val = 'google::protobuf::Value'->new();
            $val->from_perl($v);
            push @$values, $val;
        }
        return $self;
    }

    sub get_injected_methods {
        return qw(to_perl from_perl);
    }
}

1;
