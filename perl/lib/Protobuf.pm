package Protobuf;

use strict;
use warnings;

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('Protobuf', $VERSION);

{
    package Protobuf::Internal;
    XSLoader::load('Protobuf::Internal', $VERSION);
}

1;
