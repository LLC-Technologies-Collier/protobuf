# Error Propagation

Errors originating from the `upb` library or within the XS glue code must be propagated to the Perl level as exceptions.

## Mechanisms

1.  **`upb_Status`:** Many `upb` functions, especially those involved in parsing or complex operations, accept a `upb_Status*` argument. After calling such functions, the XS code MUST check the status using `upb_Status_IsOk()`.
    -   If not OK, an error message can be retrieved using `upb_Status_ErrorMessage()`. This message should be used to construct an error string for Perl.
2.  **Return Values:** Other `upb` functions indicate errors through return values (e.g., returning `NULL`). These must be checked.
3.  **`croak`:** The standard mechanism for signaling errors from XS to Perl is `croak()`. XS functions should use `croak()` to throw Perl exceptions. The message passed to `croak()` should be informative, including the error message from `upb_Status` if available.

## Example XS Error Handling:

```c
// Inside an XSUB
upb_Status status;
upb_Status_Clear(&status);
bool ok = upb_SomeOperation(..., &status);
if (!ok) {
    const char* err_msg = upb_Status_ErrorMessage(&status);
    croak("Protobuf error: %s", err_msg);
}

void* ptr = upb_AnotherOperation(...);
if (!ptr) {
    croak("Protobuf error: AnotherOperation failed");
}
```

## Perl-level Catching

Perl code using the `Protobuf` module can use `eval { ... }` blocks or modules like `Try::Tiny` to catch these exceptions:

```perl
use Try::Tiny;

try {
    my $msg = MyMessage->decode($binary_data);
} catch {
    warn "Failed to decode message: $_";
};
```

This error handling strategy is consistent with both the Ruby (using `rb_raise`) and PHP (using `zend_throw_exception_ex`) extensions, providing clear and idiomatic error reporting to the Perl user.
