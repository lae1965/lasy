#include <sldbas.h>

static void calsum (DOC * doc, FLD * pfldfrom);

void dbcalsum (DOC * doc, FLD * pfldfrom) {

    if ((dbgetidf (doc, pfldfrom))->initype != SL_SUM) return;
    calsum (doc, pfldfrom);
    return;
}

static void calsum (DOC * doc, FLD * pfldfrom) {
    FLD     * pfldto = pfldfrom;
    IDFLD   * pidf;
    int       idnto, idnfrom;

    while (1) {
        pidf = dbgetidf (doc, pfldfrom);

        if (pidf->initype != SL_SUM) return;

        doc->opt.needout = YES;
        idnfrom = pfldfrom->idn;
        idnto = pidf->fldlink.link.idndown;

        while (pfldto->idn != idnto ||
                       (dbgetidf (doc, pfldto))->fldlink.link.idnup != idnfrom)
            pfldto = pfldto->next;
        pfldto->val.d = 0.0;
        pfldto->prnopt.full = NO;

        pfldfrom = pfldto->curcrt->fld_first;
        do {
            if (pfldfrom->idn == idnfrom) {
                if (pfldfrom->prnopt.full) {
                    switch (pidf->valtype) {
                        case SL_FLOAT:
                            pfldto->val.f += pfldfrom->val.f;
                            break;
                        case SL_MONEY:
                        case SL_DOUBLE:
                            pfldto->val.d += pfldfrom->val.d;
                            break;
                        case SL_LONG:
                            pfldto->val.l += pfldfrom->val.l;
                            break;
                        case SL_INT:
                            pfldto->val.i += pfldfrom->val.i;
                            break;
                        default:  return;
                    }
                    pfldto->prnopt.full = YES;
                }
            }
            pfldfrom = pfldfrom->next;
        } while (pfldfrom != pfldto);
    }
}

