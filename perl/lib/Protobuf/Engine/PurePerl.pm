package Protobuf::Engine::PurePerl;

use strict;
use warnings;
use parent 'Protobuf::Engine';
use Carp qw(croak);

sub create_message {
    my ($self, $class, $mdef, $arena, $flags) = @_;
    
    # Internal representation for PurePerl: a HashRef
    my $data = {
        _fields => {},
        _mdef   => $mdef,
        _engine => $self,
    };
    
    return bless $data, $class;
}

sub get {
    my ($self, $msg, $field_name) = @_;
    return $msg->{_fields}{$field_name};
}

sub set {
    my ($self, $msg, $field_name, $value) = @_;
    $msg->{_fields}{$field_name} = $value;
}

sub has {
    my ($self, $msg, $field_name) = @_;
    return exists $msg->{_fields}{$field_name};
}

sub clear {
    my ($self, $msg, $field_name) = @_;
    delete $msg->{_fields}{$field_name};
}

# --- Varint Kernels ---

sub _encode_varint {
    my ($val) = @_;
    my $res = '';
    # Handle negative numbers: they are always 10 bytes in PB varint
    if ($val < 0) {
        for (1..9) {
            $res .= chr(($val & 0x7f) | 0x80);
            $val >>= 7;
        }
        $res .= chr($val & 0x7f);
        return $res;
    }
    
    while ($val >= 0x80) {
        $res .= chr(($val & 0x7f) | 0x80);
        $val >>= 7;
    }
    $res .= chr($val);
    return $res;
}

sub _decode_varint {
    my ($data_ref, $pos_ref) = @_;
    my $val = 0;
    my $shift = 0;
    while (1) {
        my $byte = ord(substr($$data_ref, $$pos_ref++, 1));
        $val |= ($byte & 0x7f) << $shift;
        last unless $byte & 0x80;
        $shift += 7;
        if ($shift >= 70) { croak "Varint too long"; }
    }
    return $val;
}

use Math::BigInt;

# --- ZigZag Kernels ---

sub _encode_zigzag32 {
    my ($val) = @_;
    # Use 32-bit arithmetic
    my $v = Math::BigInt->new($val);
    my $res = ($v->copy()->blsft(1))->bxor($v->copy()->brsft(31));
    return $res->as_number() & 0xFFFFFFFF;
}

sub _decode_zigzag32 {
    my ($val) = @_;
    my $v = Math::BigInt->new($val);
    my $res = ($v->copy()->brsft(1))->bxor($v->copy()->band(1)->copy()->bneg());
    my $final = $res->as_number();
    # Truncate to 32-bit signed
    return unpack('l', pack('l', $final));
}

sub _encode_zigzag64 {
    my ($val) = @_;
    my $v = Math::BigInt->new($val);
    my $res = ($v->copy()->blsft(1))->bxor($v->copy()->brsft(63));
    return $res->as_hex(); # Return hex to avoid precision loss on large IVs
}

sub _decode_zigzag64 {
    my ($val) = @_;
    my $v = Math::BigInt->from_hex($val) if $val =~ /^0x/;
    $v ||= Math::BigInt->new($val);
    my $res = ($v->copy()->brsft(1))->bxor($v->copy()->band(1)->copy()->bneg());
    return $res->as_number();
}

# --- Wire Format Implementation ---

my %TYPE_TO_WIRE = (
    1  => 1, # DOUBLE -> 64-bit
    2  => 5, # FLOAT -> 32-bit
    3  => 0, # INT64 -> Varint
    4  => 0, # UINT64 -> Varint
    5  => 0, # INT32 -> Varint
    6  => 1, # FIXED64 -> 64-bit
    7  => 5, # FIXED32 -> 32-bit
    8  => 0, # BOOL -> Varint
    9  => 2, # STRING -> Length-delimited
    11 => 2, # MESSAGE -> Length-delimited
    12 => 2, # BYTES -> Length-delimited
    13 => 0, # UINT32 -> Varint
    14 => 0, # ENUM -> Varint
    15 => 5, # SFIXED32 -> 32-bit
    16 => 1, # SFIXED64 -> 64-bit
    17 => 0, # SINT32 -> Varint
    18 => 0, # SINT64 -> Varint
);

sub serialize {
    my ($self, $msg) = @_;
    my $mdef = $msg->{_mdef};
    my $fields = $msg->{_fields};
    my $res = '';
    
    # Get fields from mdef (handle both XS and PurePerl mdef)
    my @field_defs;
    if ($mdef->isa('Protobuf::Descriptor::MessageDef::PurePerl')) {
        my $count = $mdef->field_count;
        for (0..$count-1) {
            my $f = $mdef->get_field($_);
            push @field_defs, {
                name => $f->name,
                number => $f->number,
                type => $f->type,
                label => $f->label,
            };
        }
    } elsif (ref($mdef) eq 'HASH') {
        @field_defs = @{$mdef->{field} || []};
    } else {
        my $count = $mdef->field_count;
        for (0..$count-1) {
            my $f = $mdef->get_field($_);
            push @field_defs, {
                name => $f->name,
                number => $f->number,
                type => $f->type,
                label => $f->label, # 1: optional, 2: required, 3: repeated
            };
        }
    }

    foreach my $f (@field_defs) {
        my $val = $fields->{$f->{name}};
        next unless defined $val;
        
        my $tag = $f->{number};
        my $type = $f->{type};
        my $wire = $TYPE_TO_WIRE{$type};
        
        if ($f->{label} == 3) { # Repeated
            # TODO: handle packed repeated
            foreach my $item (@$val) {
                $res .= $self->_encode_field($tag, $type, $wire, $item);
            }
        } else {
            $res .= $self->_encode_field($tag, $type, $wire, $val);
        }
    }
    
    return $res;
}

sub _encode_field {
    my ($self, $tag, $type, $wire, $val) = @_;
    my $res = _encode_varint(($tag << 3) | $wire);
    
    if ($wire == 0) { # Varint
        if ($type == 17) { $val = _encode_zigzag32($val); }
        elsif ($type == 18) { $val = _encode_zigzag64($val); }
        $res .= _encode_varint($val);
    } elsif ($wire == 1) { # 64-bit
        if ($type == 1) { $res .= pack('d', $val); } # double
        else { $res .= pack('Q', $val); } # fixed64, sfixed64
    } elsif ($wire == 2) { # Length-delimited
        if ($type == 11) { # Message
            my $inner = $val->serialize();
            $res .= _encode_varint(length($inner)) . $inner;
        } else { # String, Bytes
            $res .= _encode_varint(length($val)) . $val;
        }
    } elsif ($wire == 5) { # 32-bit
        if ($type == 2) { $res .= pack('f', $val); } # float
        else { $res .= pack('V', $val); } # fixed32, sfixed32
    }
    return $res;
}

sub parse {
    my ($self, $class, $data, $options) = @_;
    my $mdef = $class->descriptor;
    my $msg = $self->create_message($class, $mdef);
    my $fields = $msg->{_fields};
    
    my $pos = 0;
    my $len = length($data);
    
    # Pre-index fields by number for fast lookup
    my %fields_by_num;
    if ($mdef->isa('Protobuf::Descriptor::MessageDef::PurePerl')) {
        my $count = $mdef->field_count;
        for (0..$count-1) {
            my $f = $mdef->get_field($_);
            $fields_by_num{$f->number} = {
                name => $f->name,
                type => $f->type,
                label => $f->label,
                message_type => ($f->type == 11) ? $f->message_type : undef,
            };
        }
    } elsif (ref($mdef) eq 'HASH') {
        %fields_by_num = map { $_->{number} => $_ } @{$mdef->{field} || []};
    } else {
        my $count = $mdef->field_count;
        for (0..$count-1) {
            my $f = $mdef->get_field($_);
            $fields_by_num{$f->number} = {
                name => $f->name,
                type => $f->type,
                label => $f->label,
                message_type => ($f->type == 11) ? $f->message_type : undef,
            };
        }
    }

    while ($pos < $len) {
        my $tag_wire = _decode_varint(\$data, \$pos);
        my $tag = $tag_wire >> 3;
        my $wire = $tag_wire & 0x07;
        
        my $f = $fields_by_num{$tag};
        if (!$f) {
            # Skip unknown field
            _skip_field(\$data, \$pos, $wire);
            next;
        }
        
        my $val = $self->_decode_field(\$data, \$pos, $wire, $f);
        if ($f->{label} == 3) {
            push @{$fields->{$f->{name}} ||= []}, $val;
        } else {
            $fields->{$f->{name}} = $val;
        }
    }
    
    return $msg;
}

sub _decode_field {
    my ($self, $data_ref, $pos_ref, $wire, $f) = @_;
    my $type = $f->{type};
    
    if ($wire == 0) { # Varint
        my $val = _decode_varint($data_ref, $pos_ref);
        if ($type == 17) { return _decode_zigzag32($val); }
        if ($type == 18) { return _decode_zigzag64($val); }
        return $val;
    } elsif ($wire == 1) { # 64-bit
        my $bytes = substr($$data_ref, $$pos_ref, 8);
        $$pos_ref += 8;
        if ($type == 1) { return unpack('d', $bytes); }
        return unpack('Q', $bytes);
    } elsif ($wire == 2) { # Length-delimited
        my $len = _decode_varint($data_ref, $pos_ref);
        my $bytes = substr($$data_ref, $$pos_ref, $len);
        $$pos_ref += $len;
        if ($type == 11) { # Message
            my $sub_class = $self->_get_perl_class_for_mdef($f->{message_type});
            return $sub_class->parse($bytes);
        }
        return $bytes;
    } elsif ($wire == 5) { # 32-bit
        my $bytes = substr($$data_ref, $$pos_ref, 4);
        $$pos_ref += 4;
        if ($type == 2) { return unpack('f', $bytes); }
        return unpack('V', $bytes);
    }
}

sub _get_perl_class_for_mdef {
    my ($self, $mdef) = @_;
    return undef unless $mdef;
    
    # XS mdef
    return $mdef->perl_class_name() if $mdef->can('perl_class_name');
    
    # PurePerl mdef (Class)
    if ($mdef->isa('Protobuf::Descriptor::MessageDef::PurePerl')) {
        return $mdef->perl_class_name();
    }
    
    # Fallback for PurePerl mdef (HashRef)
    return $mdef->{perl_class};
}

# Helper from DescriptorParser (we should probably move these to a common Utility module)
sub _skip_field {
    my ($data_ref, $pos_ref, $wire) = @_;
    if ($wire == 0) { _decode_varint($data_ref, $pos_ref); }
    elsif ($wire == 1) { $$pos_ref += 8; }
    elsif ($wire == 2) {
        my $len = _decode_varint($data_ref, $pos_ref);
        $$pos_ref += $len;
    }
    elsif ($wire == 5) { $$pos_ref += 4; }
    else { croak "Unsupported wire type $wire"; }
}

sub merge {
    my ($self, $dst, $src) = @_;
    # TODO
    croak "PurePerl merge not yet implemented";
}

sub copy {
    my ($self, $dst, $src) = @_;
    # TODO
    croak "PurePerl copy not yet implemented";
}

sub to_json {
    my ($self, $msg) = @_;
    # TODO
    croak "PurePerl JSON encoder not yet implemented";
}

sub from_json {
    my ($self, $class, $json) = @_;
    # TODO
    croak "PurePerl JSON decoder not yet implemented";
}

sub to_perl {
    my ($self, $msg) = @_;
    my $fields = $msg->{_fields};
    my $res = {};
    
    foreach my $name (keys %$fields) {
        my $val = $fields->{$name};
        if (ref($val)) {
            if ($val->isa('Protobuf::Message')) {
                $res->{$name} = $val->to_perl();
            } elsif (ref($val) eq 'ARRAY') {
                $res->{$name} = [ map { ref($_) && $_->isa('Protobuf::Message') ? $_->to_perl() : $_ } @$val ];
            } elsif (ref($val) eq 'HASH') {
                # Map
                my $map_res = {};
                while (my ($k, $v) = each %$val) {
                    $map_res->{$k} = (ref($v) && $v->isa('Protobuf::Message')) ? $v->to_perl() : $v;
                }
                $res->{$name} = $map_res;
            } else {
                $res->{$name} = $val;
            }
        } else {
            $res->{$name} = $val;
        }
    }
    return $res;
}

sub to_text {
    my ($self, $msg) = @_;
    return "PurePerl text format stub";
}

1;
