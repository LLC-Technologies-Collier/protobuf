#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/arena.h"
#include "upb/mem/arena.h"
#include "upb/mem/alloc.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Generalized Linear allocator
static void* PerlUpb_BlockAlloc_Func(upb_alloc* alloc, void* ptr, size_t oldsize,
                                     size_t size, size_t* actual_size) {
    PerlUpb_BlockAlloc* b = (PerlUpb_BlockAlloc*)alloc;

    if (size == 0) {
        if (ptr != NULL) PerlUpb_VerifyCanaries(ptr, oldsize, "BlockAlloc free");
        return NULL;
    }

    if (ptr != NULL) {
        // Realloc
        PerlUpb_VerifyCanaries(ptr, oldsize, "BlockAlloc realloc");
        void* new_ptr = PerlUpb_BlockAlloc_Func(alloc, NULL, 0, size, actual_size);
        if (new_ptr && oldsize > 0) {
            memcpy(new_ptr, ptr, oldsize);
        }
        return new_ptr;
    }

    // Alignment (16 bytes for canaries)
    size_t requested_size = size + 2 * PERL_UPB_CANARY_SIZE;
    size_t aligned_size = (requested_size + 15) & ~15;
    if (b->offset + aligned_size > b->size) return NULL;

    void* raw = (char*)b->region + b->offset;
    PerlUpb_WriteCanaries(raw, size);
    void* ret = (char*)raw + PERL_UPB_CANARY_SIZE;
    
    b->offset += aligned_size;
    
    if (actual_size) *actual_size = size;
    return ret;
}

SV* PerlUpb_Arena_NewTmpfs(pTHX_ const char* path, size_t size) {
    int fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd < 0) croak("Failed to open tmpfs file %s: %s", path, strerror(errno));

    if (ftruncate(fd, size) != 0) {
        close(fd);
        croak("Failed to truncate tmpfs file %s: %s", path, strerror(errno));
    }

    void* region = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
        close(fd);
        croak("Failed to mmap tmpfs file %s: %s", path, strerror(errno));
    }

    PerlUpb_BlockAlloc* b_alloc = (PerlUpb_BlockAlloc*)safemalloc(sizeof(PerlUpb_BlockAlloc));
    b_alloc->base.func = PerlUpb_BlockAlloc_Func;
    b_alloc->type = PERL_UPB_BLOCK_MMAP;
    b_alloc->fd = fd;
    b_alloc->region = region;
    b_alloc->size = size;
    b_alloc->offset = 0;

    upb_Arena* arena = upb_Arena_Init(NULL, 0, &b_alloc->base);
    if (!arena) {
        munmap(region, size);
        close(fd);
        safefree(b_alloc);
        croak("Failed to initialize upb_Arena with block allocator");
    }

    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)safemalloc(sizeof(PerlUpb_Arena_Custom));
    wrapper->base.arena = arena;
    wrapper->alloc = b_alloc;

    HV* hv = newHV();
    hv_store(hv, "_arena_ptr", 10, newSViv(PTR2IV(wrapper)), 0);
    hv_store(hv, "_is_tmpfs", 9, newSViv(1), 0);

    SV* rv = newRV_noinc((SV*)hv);
    sv_bless(rv, gv_stashpv("Protobuf::Arena", GV_ADD));
    return rv;
}

// Internal helper for RAM-backed linear arenas
upb_Arena* PerlUpb_Arena_NewBlock(pTHX_ size_t size, PerlUpb_BlockAlloc** out_alloc) {
    void* region = safemalloc(size);
    
    PerlUpb_BlockAlloc* b_alloc = (PerlUpb_BlockAlloc*)safemalloc(sizeof(PerlUpb_BlockAlloc));
    b_alloc->base.func = PerlUpb_BlockAlloc_Func;
    b_alloc->type = PERL_UPB_BLOCK_MALLOC;
    b_alloc->fd = -1;
    b_alloc->region = region;
    b_alloc->size = size;
    b_alloc->offset = 0;

    upb_Arena* arena = upb_Arena_Init(NULL, 0, &b_alloc->base);
    if (!arena) {
        safefree(region);
        safefree(b_alloc);
        return NULL;
    }

    if (out_alloc) *out_alloc = b_alloc;
    return arena;
}

void PerlUpb_Arena_DestroyRaw_Tmpfs(pTHX_ void* ptr, bool is_tmpfs) {
    if (!is_tmpfs) {
        PerlUpb_Arena_DestroyRaw(aTHX_ ptr);
        return;
    }

    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)ptr;
    if (wrapper) {
        if (wrapper->base.arena) upb_Arena_Free(wrapper->base.arena);
        if (wrapper->alloc) {
            if (wrapper->alloc->type == PERL_UPB_BLOCK_MMAP) {
                munmap(wrapper->alloc->region, wrapper->alloc->size);
                close(wrapper->alloc->fd);
            } else {
                safefree(wrapper->alloc->region);
            }
            safefree(wrapper->alloc);
        }
        safefree(wrapper);
    }
}
