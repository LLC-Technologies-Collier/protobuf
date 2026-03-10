use strict;
use warnings;
use Test::More;

if ( not $ENV{TEST_AUTHOR} ) {
    my $msg = 'Author test.  Set $ENV{TEST_AUTHOR} to a true value to run.';
    plan( skip_all => $msg );
}

eval "use Test::Pod 1.14";  # Or a more recent version if needed

if ( $@ ) {
    my $msg = 'Test::Pod 1.14 required for testing POD';
    plan( skip_all => $msg );
}

all_pod_files_ok();
