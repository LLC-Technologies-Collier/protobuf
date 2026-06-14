#!/usr/bin/env perl
use strict;
use warnings;
use Test::More;
use Test::Pod::Coverage 1.08;

# Find all modules in the package
my @modules = all_modules();

# Allowlist only the core public API modules that are designed to be fully documented
my @public_modules = grep {
    /^Protobuf$/ ||
    /^Protobuf::WKT/ ||
    /^Protobuf::UnknownFieldSet$/
} @modules;

# Plan and run coverage checks only on the allowlisted public API
plan tests => scalar @public_modules;

foreach my $module (@public_modules) {
    if ($module eq 'Protobuf') {
        pod_coverage_ok($module, { trustme => [qr/^(?:engine|get_engine)$/] });
    } else {
        pod_coverage_ok($module);
    }
}
