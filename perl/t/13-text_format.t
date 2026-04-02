use strict;
use warnings;
use Test::More;
use lib "t/lib";
use TestHelpers;

my $pool = TestHelpers->get_generated_pool();
TestHelpers->load_test_protos($pool, 't/data/test_descriptor.bin');

subtest 'text format encoding' => sub {
    my $msg = test::TestMessage->new();
    $msg->set_value(12345);
    $msg->set_name("hello world");
    
    my $sub = test::NestedMessage->new();
    $sub->set_nested_string("inner");
    $msg->set_nested_message($sub);
    
    # We expect some methods on Protobuf::Message
    my $text = $msg->to_text();
    ok(defined $text, 'Text format is generated');
    like($text, qr/value: 12345/, 'Contains integer field');
    like($text, qr/name: "hello world"/, 'Contains string field');
    like($text, qr/nested_message \{.*nested_string: "inner".*\}/s, 'Contains nested message');

    subtest 'text format with unknowns' => sub {
        my $msg2 = test::TestMessage->new();
        my $text2 = $msg2->to_text_with_unknowns();
        ok(defined($text2), 'Got text with unknowns');
    };
};

TODO: {
    local $TODO = 'Implement Direct-to-Stream Text Printing (C-Layer)';
    ok(0, 'Printing text format directly to file handle avoids intermediate Perl strings');
}

TODO: {
    local $TODO = 'Implement Lossless Text Formatting for Unknown Fields';
    ok(0, 'Unknown fields are included in text output with structured representation');
}

TODO: {
    local $TODO = 'Provide Custom Text Format Redactors';
    ok(0, 'Specific fields can be redacted during text formatting via callbacks');
}

done_testing();
