use strict;
use warnings;
use Test::More;
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

done_testing();
