use strict;
use warnings;
use Test::More;
use Protobuf;
use Protobuf::DescriptorPool;
use Path::Tiny;

# Use the compat_descriptor.bin for tests
my $pool = Protobuf::DescriptorPool->generated_pool;
my $bin = path('t/data/compat_descriptor.bin')->slurp_raw;
$pool->add_serialized_file_descriptor_set($bin);

subtest 'Type::Tiny validation in setters' => sub {
    my $msg = String::StringBytes::Bytes->new();
    
    # Valid set
    eval { $msg->v_string('hello') };
    ok(!$@, 'Valid string set successfully') or diag($@);
    is($msg->v_string, 'hello', 'Value is correct');

    # Invalid set (Number instead of string - wait, Perl might auto-convert)
    # Let's try something that definitely fails Str check if it's strict
    # Actually Type::Standard Str accepts numbers.
    # Let's try a HashRef where a string is expected.
    eval { $msg->v_string({ a => 1 }) };
    ok($@, 'Invalid set (HashRef to Str) fails') or diag("Should have failed but didn't");
    like($@, qr/did not pass type constraint "Str"/, 'Error message is correct');
};

subtest 'Type::Tiny coercion in setters (sub-messages)' => sub {
    # Protobuf::Types::Types is generated from types.proto
    my $msg = Protobuf::Types::Types->new();
    
    # Set sub-message using HashRef (coercion)
    eval {
        $msg->opt_message({ t_string => 'nested' });
    };
    ok(!$@, 'Coercion from HashRef successful') or diag($@);
    isa_ok($msg->opt_message, 'Protobuf::Types::Types::Message');
    is($msg->opt_message->t_string, 'nested', 'Nested value correct');
};

done_testing;
