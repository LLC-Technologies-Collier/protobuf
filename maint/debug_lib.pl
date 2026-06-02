use strict;
use warnings;
use lib "lib";
use Protobuf::ClassGenerator;
use Protobuf::DescriptorPool;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

my $file = $pool->find_file_by_name('perl/t/c/test.proto') || $pool->find_file_by_name('test.proto');
if (!$file) {
    die "Could not find test.proto";
}

print "INC: " . $INC{'Protobuf/ClassGenerator.pm'} . "\n";
print "CAN: " . (Protobuf::ClassGenerator->can('generate_type_library') ? "YES" : "NO") . "\n";

if (Protobuf::ClassGenerator->can('generate_type_library')) {
    my $code = Protobuf::ClassGenerator->generate_type_library($file);
    print "CODE LENGTH: " . length($code) . "\n";
    print "SUBSTR: " . substr($code, 0, 100) . "...\n";
}
