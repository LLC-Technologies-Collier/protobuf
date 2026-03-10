#!/usr/bin/env perl

use strict;
use warnings;
use Test::More skip_all => 1;
use Test::Pod::Coverage 1.08;

all_pod_coverage_ok();
