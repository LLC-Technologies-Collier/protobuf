# Repeated Container

_Status: C Layer Implemented_

The `Protobuf::Repeated` class provides an array-like interface for accessing and modifying repeated fields in a message.

## Implementation

The C implementation is located in `perl/xs/repeated/`.

*   **`repeated.c`**: Implements the main repeated operations.
    *   `PerlUpb_Repeated_New`: Creates a new repeated wrapper for a given `upb_Array`. It holds a strong reference to the arena SV to ensure the array's memory stays alive.
    *   `PerlUpb_Repeated_GetItem`: Retrieves the value at a given index. Uses `PerlUpb_UpbToSv_Element` to ensure correct conversion.
    *   `PerlUpb_Repeated_SetItem`: Sets the value at a given index. Uses `PerlUpb_SvToUpb_Element`.
    *   `PerlUpb_Repeated_Append`: Appends a value to the end of the array.
    *   `PerlUpb_Repeated_Size`: Returns the number of entries via `upb_Array_Size`.
    *   `PerlUpb_Repeated_Delete`: Removes entries from the array via `upb_Array_Delete`.
*   **`composite.c`**: Implements specialized logic for message/group repeated fields.
    *   `PerlUpb_Repeated_Add`: Creates a new submessage on the arena, appends it to the array, and returns a Perl wrapper. Correctly uses `PerlUpb_ObjCache` for the returned object.

## Object Identity and Caching

The `Repeated` container leverages `PerlUpb_ObjCache` via `PerlUpb_WrapMessage`. When accessing a repeated message field multiple times, or via different paths, the same Perl SV wrapper is returned for the same underlying `upb_Message` pointer.

## Memory Management

*   The `Repeated` object holds a strong reference to the `arena_sv` of the parent message.
*   This ensures that the underlying `upb_Array` remains valid as long as the Perl wrapper is in use.
