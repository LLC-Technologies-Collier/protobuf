#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "perl/xs/protobuf/registry.h"

#define REGISTRY_KEY "Protobuf::Registry"

void PerlUpb_Registry_Init(pTHX) {
    SV** svp = hv_fetch(PL_modglobal, REGISTRY_KEY, strlen(REGISTRY_KEY), 1);
    if (!svp) {
        croak("Failed to initialize Protobuf registry in PL_modglobal");
    }

    if (!SvIOK(*svp)) {
        PerlUpb_Registry* reg = (PerlUpb_Registry*)safemalloc(sizeof(PerlUpb_Registry));
        memset(reg, 0, sizeof(PerlUpb_Registry));
        
        // Default capacity
        reg->max_cache_capacity = 100000;
        
        sv_setiv(*svp, PTR2IV(reg));
    }
}

PerlUpb_Registry* PerlUpb_Registry_Get(pTHX) {
    SV** svp = hv_fetch(PL_modglobal, REGISTRY_KEY, strlen(REGISTRY_KEY), 0);
    if (svp && SvIOK(*svp)) {
        return INT2PTR(PerlUpb_Registry*, SvIV(*svp));
    }
    
    // Auto-init if missing (lazy)
    PerlUpb_Registry_Init(aTHX);
    svp = hv_fetch(PL_modglobal, REGISTRY_KEY, strlen(REGISTRY_KEY), 0);
    return INT2PTR(PerlUpb_Registry*, SvIV(*svp));
}
