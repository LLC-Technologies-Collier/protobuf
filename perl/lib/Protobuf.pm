package Protobuf;

use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('Protobuf', $VERSION);

use Protobuf::Internal;
Protobuf::Internal::init_registry();

1;
