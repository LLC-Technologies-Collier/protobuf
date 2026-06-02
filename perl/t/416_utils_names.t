use strict;
use warnings;
use Test::More;
use Protobuf;
plan skip_all => 'XS required for internal utility tests' unless $Protobuf::HAS_XS;
use Protobuf::Internal;

# 1. ClassNameToFullName (with SIMD path)
is(Protobuf::Internal::class_name_to_full_name("My::Package::Message"), "My.Package.Message", "Standard class name");
is(Protobuf::Internal::class_name_to_full_name("Short"), "Short", "No colons");
is(Protobuf::Internal::class_name_to_full_name("Very::Long::Package::Name::That::Exceeds::Sixteen::Bytes::Limit"), 
   "Very.Long.Package.Name.That.Exceeds.Sixteen.Bytes.Limit", "Long class name (SIMD path)");

# 2. FullNameToClassName
is(Protobuf::Internal::full_name_to_class_name("My.Package.Message"), "My::Package::Message", "Standard full name");
is(Protobuf::Internal::full_name_to_class_name("Short"), "Short", "No dots");
is(Protobuf::Internal::full_name_to_class_name("Very.Long.Package.Name.That.Exceeds.Sixteen.Bytes.Limit"),
   "Very::Long::Package::Name::That::Exceeds::Sixteen::Bytes::Limit", "Long full name");

# 3. CPU Features
my $features = Protobuf::Internal::get_cpu_features();
ok(defined $features, "get_cpu_features returns a value ($features)");

if ($features & 0x01) {
    note("CPU supports SSE4.1");
}
if ($features & 0x02) {
    note("CPU supports AVX2");
}

done_testing();
