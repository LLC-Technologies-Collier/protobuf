#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "xs/protobuf/arena.h"

// -- Arena --
MODULE = Protobuf::Arena  PACKAGE = Protobuf::Arena
PROTOTYPES: ENABLE


IV
_xs_create_raw()
    CODE:
        RETVAL = PTR2IV(PerlUpb_Arena_CreateRaw(aTHX));
    OUTPUT:
        RETVAL

SV*
_xs_create_tmpfs_raw(path, size)
    const char* path
    size_t size
    CODE:
        RETVAL = PerlUpb_Arena_NewTmpfs(aTHX_ path, size);
    OUTPUT:
        RETVAL

void
_xs_destroy_raw(ptr_iv)
    IV ptr_iv
    CODE:
        PerlUpb_Arena_DestroyRaw(aTHX_ INT2PTR(void*, ptr_iv));

void
_xs_destroy(self)
    SV* self
    CODE:
        PerlUpb_Arena_Destroy(aTHX_ self);

UV
space_allocated(self)
    SV* self
    CODE:
        RETVAL = (UV)PerlUpb_Arena_SpaceAllocated(aTHX_ self);
    OUTPUT:
        RETVAL

UV
space_reserved(self)
    SV* self
    CODE:
        RETVAL = (UV)PerlUpb_Arena_SpaceReserved(aTHX_ self);
    OUTPUT:
        RETVAL

SV*
stats(self)
    SV* self
    PREINIT:
        PerlUpb_ArenaStats stats;
    CODE:
        PerlUpb_Arena_GetStats(aTHX, self, &stats);
        HV* hv = newHV();
        hv_store(hv, "allocated", 9, newSVuv(stats.allocated), 0);
        hv_store(hv, "reserved", 8, newSVuv(stats.reserved), 0);
        hv_store(hv, "blocks", 6, newSVuv(stats.blocks), 0);
        RETVAL = newRV_noinc((SV*)hv);
    OUTPUT:
        RETVAL

void
_xs_test_canary_corruption(self)
    SV* self
    CODE:
        upb_Arena* arena = PerlUpb_Arena_Get(aTHX_ self);
        size_t size = 64;
        char* p = upb_Arena_Malloc(arena, size);
        // Corruption: Overwrite the end canary.
        // The end canary starts exactly at p + size.
        uint64_t* end_canary = (uint64_t*)(p + size);
        end_canary[0] = 0xBAD0BAD0BAD0BAD0ULL;
        // Note: Corruption is detected during free/realloc.

void
_xs_test_block_canary(self)
    SV* self
    CODE:
        SV* rv = SvRV(self);
        SV** svp = hv_fetch((HV*)rv, "_arena_ptr", 10, 0);
        if (!svp || !SvIOK(*svp)) croak("Invalid arena object");
        void* raw_ptr = INT2PTR(void*, SvIV(*svp));
        PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)raw_ptr;
        size_t size = 64;
        void* p = wrapper->alloc->base.func(&wrapper->alloc->base, NULL, 0, size, NULL);
        if (p) {
            uint64_t* end_canary = (uint64_t*)((char*)p + size);
            end_canary[0] = 0xBAD0BAD0BAD0BAD0ULL;
            // Trigger check
            wrapper->alloc->base.func(&wrapper->alloc->base, p, size, 0, NULL);
        }

