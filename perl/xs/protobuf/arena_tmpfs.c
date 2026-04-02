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

typedef struct {
    upb_alloc base;
    int fd;
    void* region;
    size_t size;
    size_t offset;
    bool is_owner;
} PerlUpb_TmpfsAlloc;

// Linear allocator from mmap region
static void* PerlUpb_TmpfsAlloc_Func(upb_alloc* alloc, void* ptr, size_t oldsize,
                                     size_t size, size_t* actual_size) {
    PerlUpb_TmpfsAlloc* t = (PerlUpb_TmpfsAlloc*)alloc;

    if (size == 0) {
        // Free is a no-op for this linear allocator; arena owns everything.
        return NULL;
    }

    if (ptr != NULL) {
        // Realloc: we only support appending. If ptr is the last allocation, 
        // we could theoretically expand it, but for simplicity, we just 
        // allocate new space and copy.
        void* new_ptr = PerlUpb_TmpfsAlloc_Func(alloc, NULL, 0, size, actual_size);
        if (new_ptr && oldsize > 0) {
            memcpy(new_ptr, ptr, oldsize);
        }
        return new_ptr;
    }

    // Alignment (8 bytes)
    size_t aligned_size = (size + 7) & ~7;
    if (t->offset + aligned_size > t->size) {
        return NULL; // OOM in shared region
    }

    void* ret = (char*)t->region + t->offset;
    t->offset += aligned_size;
    
    if (actual_size) {
        *actual_size = aligned_size;
    }

    return ret;
}

// Special wrapper for Tmpfs Arena
typedef struct {
    PerlUpb_Arena base;
    PerlUpb_TmpfsAlloc* alloc;
} PerlUpb_Arena_Tmpfs;

SV* PerlUpb_Arena_NewTmpfs(pTHX_ const char* path, size_t size) {
    int fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        croak("Failed to open tmpfs file %s: %s", path, strerror(errno));
    }

    // Ensure file is large enough
    if (ftruncate(fd, size) != 0) {
        close(fd);
        croak("Failed to truncate tmpfs file %s to %zu: %s", path, size, strerror(errno));
    }

    void* region = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
        close(fd);
        croak("Failed to mmap tmpfs file %s: %s", path, strerror(errno));
    }

    PerlUpb_TmpfsAlloc* t_alloc = (PerlUpb_TmpfsAlloc*)safemalloc(sizeof(PerlUpb_TmpfsAlloc));
    t_alloc->base.func = PerlUpb_TmpfsAlloc_Func;
    t_alloc->fd = fd;
    t_alloc->region = region;
    t_alloc->size = size;
    t_alloc->offset = 0;
    t_alloc->is_owner = true;

    upb_Arena* arena = upb_Arena_Init(NULL, 0, &t_alloc->base);
    if (!arena) {
        munmap(region, size);
        close(fd);
        safefree(t_alloc);
        croak("Failed to initialize upb_Arena with tmpfs allocator");
    }

    PerlUpb_Arena_Tmpfs* wrapper = (PerlUpb_Arena_Tmpfs*)safemalloc(sizeof(PerlUpb_Arena_Tmpfs));
    wrapper->base.arena = arena;
    wrapper->alloc = t_alloc;

    HV* hv = newHV();
    hv_store(hv, "_arena_ptr", 10, newSViv(PTR2IV(wrapper)), 0);
    // Flag it as a tmpfs arena for special cleanup if needed, though 
    // PerlUpb_Arena_Destroy will be updated to handle it.
    hv_store(hv, "_is_tmpfs", 9, newSViv(1), 0);

    SV* rv = newRV_noinc((SV*)hv);
    sv_bless(rv, gv_stashpv("Protobuf::Arena", GV_ADD));
    return rv;
}

// Update DestroyRaw to handle PerlUpb_Arena_Tmpfs
void PerlUpb_Arena_DestroyRaw_Tmpfs(pTHX_ void* ptr, bool is_tmpfs) {
    if (!is_tmpfs) {
        PerlUpb_Arena_DestroyRaw(aTHX_ ptr);
        return;
    }

    PerlUpb_Arena_Tmpfs* wrapper = (PerlUpb_Arena_Tmpfs*)ptr;
    if (wrapper) {
        if (wrapper->base.arena) {
            upb_Arena_Free(wrapper->base.arena);
        }
        if (wrapper->alloc) {
            munmap(wrapper->alloc->region, wrapper->alloc->size);
            close(wrapper->alloc->fd);
            safefree(wrapper->alloc);
        }
        safefree(wrapper);
    }
}
