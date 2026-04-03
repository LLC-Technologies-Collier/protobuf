#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/arena.h"
#include "perl/xs/protobuf/message.h"
#include "perl/xs/descriptor_pool/pool.h"
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

    if (b->poisoned) return NULL;

    if (size == 0) {
        if (ptr != NULL) PerlUpb_VerifyCanaries(ptr, oldsize, "BlockAlloc free", &b->poisoned);
        return NULL;
    }

    if (ptr != NULL) {
        // Realloc
        PerlUpb_VerifyCanaries(ptr, oldsize, "BlockAlloc realloc", &b->poisoned);
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
    b_alloc->path = savepv(path);
    b_alloc->region = region;
    b_alloc->size = size;
    b_alloc->offset = 0;

    upb_Arena* arena = upb_Arena_Init(NULL, 0, &b_alloc->base);
    if (!arena) {
        munmap(region, size);
        close(fd);
        safefree(b_alloc->path);
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
    b_alloc->path = NULL;
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

SV* PerlUpb_Arena_AttachTmpfs(pTHX_ const char* path, size_t size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) croak("Failed to open existing tmpfs file %s: %s", path, strerror(errno));

    // For read-only attachment, we mmap the existing data
    void* region = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
        close(fd);
        croak("Failed to mmap tmpfs file (read-only) %s: %s", path, strerror(errno));
    }

    PerlUpb_BlockAlloc* b_alloc = (PerlUpb_BlockAlloc*)safemalloc(sizeof(PerlUpb_BlockAlloc));
    b_alloc->base.func = PerlUpb_BlockAlloc_Func;
    b_alloc->type = PERL_UPB_BLOCK_MMAP;
    b_alloc->fd = fd;
    b_alloc->path = savepv(path);
    b_alloc->region = region;
    b_alloc->size = size;
    b_alloc->offset = size; // Effectively "full" for allocation, but allows reading

    // Use global allocator for the arena structure itself
    upb_Arena* arena = upb_Arena_Init(NULL, 0, &upb_alloc_global);
    if (!arena) {
        munmap(region, size);
        close(fd);
        safefree(b_alloc->path);
        safefree(b_alloc);
        croak("Failed to create upb_Arena for attachment");
    }

    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)safemalloc(sizeof(PerlUpb_Arena_Custom));
    wrapper->base.arena = arena;
    wrapper->alloc = b_alloc;

    HV* hv = newHV();
    hv_store(hv, "_arena_ptr", 10, newSViv(PTR2IV(wrapper)), 0);
    hv_store(hv, "_is_tmpfs", 9, newSViv(1), 0);
    hv_store(hv, "_read_only", 10, newSViv(1), 0);

    SV* rv = newRV_noinc((SV*)hv);
    sv_bless(rv, gv_stashpv("Protobuf::Arena", GV_ADD));
    return rv;
}

void PerlUpb_Arena_DestroyRaw_Tmpfs(pTHX_ void* ptr, bool is_tmpfs) {
    if (!is_tmpfs) {
        PerlUpb_Arena_DestroyRaw(aTHX_ ptr);
        return;
    }

    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)ptr;
    if (wrapper) {
        if (wrapper->base.arena) PerlUpb_Arena_Release(aTHX_ wrapper->base.arena, PERL_UPB_LIFECYCLE_PERMANENT);
        if (wrapper->alloc) {
            if (wrapper->alloc->type == PERL_UPB_BLOCK_MMAP) {
                munmap(wrapper->alloc->region, wrapper->alloc->size);
                close(wrapper->alloc->fd);
                if (wrapper->alloc->path) safefree(wrapper->alloc->path);
            } else {
                safefree(wrapper->alloc->region);
            }
            safefree(wrapper->alloc);
        }
        safefree(wrapper);
    }
}

bool PerlUpb_Arena_IsTmpfs(pTHX_ SV* arena_sv) {
    if (!arena_sv || !SvROK(arena_sv)) return false;
    HV* hv = (HV*)SvRV(arena_sv);
    SV** svp = hv_fetch(hv, "_is_tmpfs", 9, 0);
    return svp && SvIV(*svp);
}

const char* PerlUpb_Arena_GetPath(pTHX_ SV* arena_sv) {
    if (!PerlUpb_Arena_IsTmpfs(aTHX_ arena_sv)) return NULL;
    HV* hv = (HV*)SvRV(arena_sv);
    SV** svp = hv_fetch(hv, "_arena_ptr", 10, 0);
    if (!svp) return NULL;
    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)SvIV(*svp);
    return wrapper->alloc->path;
}

bool PerlUpb_Arena_VerifySELinux(pTHX_ SV* arena_sv) {
    const char* path = PerlUpb_Arena_GetPath(aTHX_ arena_sv);
    if (!path) return true; 
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISREG(st.st_mode);
}

size_t PerlUpb_Arena_GetOffset(pTHX_ SV* arena_sv, void* ptr) {
    if (!PerlUpb_Arena_IsTmpfs(aTHX_ arena_sv)) return 0;
    HV* hv = (HV*)SvRV(arena_sv);
    SV** svp = hv_fetch(hv, "_arena_ptr", 10, 0);
    if (!svp) return 0;
    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)SvIV(*svp);
    if (ptr < wrapper->alloc->region || (char*)ptr >= (char*)wrapper->alloc->region + wrapper->alloc->size) {
        return 0;
    }
    return (char*)ptr - (char*)wrapper->alloc->region;
}

SV* PerlUpb_Arena_AttachMessage(pTHX_ SV* arena_sv, const char* name, size_t offset) {
    if (!PerlUpb_Arena_IsTmpfs(aTHX_ arena_sv)) croak("Arena is not a tmpfs arena");
    HV* hv = (HV*)SvRV(arena_sv);
    SV** svp = hv_fetch(hv, "_arena_ptr", 10, 0);
    PerlUpb_Arena_Custom* wrapper = (PerlUpb_Arena_Custom*)SvIV(*svp);
    if (offset >= wrapper->alloc->size) croak("Offset %zu out of bounds for arena size %zu", offset, wrapper->alloc->size);
    const upb_DefPool* pool = PerlUpb_DescriptorPool_GetPool(aTHX_ PerlUpb_DescriptorPool_GeneratedPool(aTHX));
    const upb_MessageDef* mdef = upb_DefPool_FindMessageByName(pool, name);
    if (!mdef) croak("Message definition not found: %s", name);
    upb_Message* msg = (upb_Message*)((char*)wrapper->alloc->region + offset);
    return PerlUpb_WrapMessage(aTHX_ msg, mdef, arena_sv);
}
