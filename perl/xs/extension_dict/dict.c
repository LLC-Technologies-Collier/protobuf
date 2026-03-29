#include <sys/types.h>
#include <setjmp.h>
#include <stdlib.h>

#include "xs/extension_dict/dict.h"
#include "xs/protobuf/obj_cache.h"

typedef struct {
    SV* message_sv;
} PerlUpb_ExtensionDict;

SV* PerlUpb_ExtensionDict_New(pTHX_ SV* message_sv) {
    PerlUpb_ExtensionDict* dict = (PerlUpb_ExtensionDict*)malloc(sizeof(PerlUpb_ExtensionDict));
    dict->message_sv = newSVsv(message_sv);

    SV* sv = newSViv((IV)dict);
    SV* obj = newRV_noinc(sv);
    sv_bless(obj, gv_stashpv("Protobuf::ExtensionDict", GV_ADD));
    return obj;
}

static PerlUpb_ExtensionDict* GetDict(pTHX_ SV* sv) {
    if (!sv || !SvROK(sv) || !sv_derived_from(sv, "Protobuf::ExtensionDict")) {
        return NULL;
    }
    return (PerlUpb_ExtensionDict*)SvIV(SvRV(sv));
}

SV* PerlUpb_ExtensionDict_GetItem(pTHX_ SV* self, SV* field_sv) {
    // TODO: Implement using upb_Message_Get
    return &PL_sv_undef;
}

void PerlUpb_ExtensionDict_SetItem(pTHX_ SV* self, SV* field_sv, SV* value_sv) {
    // TODO: Implement using upb_Message_Mutable
}

void PerlUpb_ExtensionDict_Free(pTHX_ SV* sv) {
    PerlUpb_ExtensionDict* dict = GetDict(aTHX_ sv);
    if (dict) {
        SvREFCNT_dec(dict->message_sv);
        free(dict);
        sv_setiv(SvRV(sv), 0);
    }
}
