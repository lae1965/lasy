#include <alloc.h>
#include <sldbas.h>

void             instrcorrect (FLD * pfld, IDLIN * pidl, int instr);

static  void     makedup      (CRTWRK * pcrtwrk);
static  void     tailcorrect  (FLD * first, FLD * last, int places, int idc);
static  void     darkinsert   (CRTWRK * pcrt, CRTWRK * pdrk, register numdark);
static  void     darkinsert   (CRTWRK * pcrtwrk, CRTWRK * pdrkwrk, int numdark);
static  void     darkcorrect  (CRTWRK * pcrtwrk, IDLIN * pidl);
static  void     boundcorrect (CRTWRK * pcrtwrk, CRT * pcrtlact);
static  bool     nextline     (CRT * pcrthigh, FLD * endcrtmains);
static  FLD    * getfldend    (CRT * pcrt);
static  CRTWRK * crtout       (CRT * pcrt);

static  DOC   * doc;

CRTWRK * dbcrtout (DOC * pdoc, CRT * crtmain) {
    CRTWRK    * pcrtwrk;

    doc     = pdoc;

    pcrtwrk = crtout (crtmain);
    pcrtwrk->doc = doc;
    if (pcrtwrk->visi) {
        pcrtwrk->nline =
            dbcalcln (doc, pcrtwrk->crtmain->fld_first,
                                                getfldend (pcrtwrk->crtmain));
        makedup (pcrtwrk);
    }
    return pcrtwrk;
}

static void makedup (CRTWRK * pcrtwrk) {
    IDLIN    * pidd;
    FLD      * pftmp;
    register   begidc = pcrtwrk->crtmain->idc;

    if (pcrtwrk->crtmain->opt.dark) return;

    pidd = &doc->idc [begidc].idd;

    if (pcrtwrk->firstdark != NULL)  darkcorrect (pcrtwrk, pidd);

    pftmp = pcrtwrk->crtmain->fld_first;
    instrcorrect (pftmp, pidd, 1);
    pftmp->opt.first = YES;
    (firstvisi (pftmp))->idc = begidc;

    return;
}

static CRTWRK  * crtout (CRT * pcrt) {
    CRTWRK  * pcrtwrk;
    CRT     * pctmp;
    FLD     * pftmp;
    int       minidc;

    while (pcrt->prev != NULL) pcrt = pcrt->prev;
    pftmp = pcrt->fld_first;
    pcrtwrk = utalloc (CRTWRK);
    pcrtwrk->crtmain = pcrt;
    pcrtwrk->fld_first = pftmp;

    if (!doc->opt.visibl || !pcrt->opt.visibl) return pcrtwrk;
    pcrtwrk->visi = YES;


    pcrtwrk->optfirst = pftmp->opt.first;
    pftmp = firstvisi (pftmp);
    pcrtwrk->beginstr = pftmp->instr;

    while (pftmp->instr != 1) pftmp = prev (pftmp);
    pcrtwrk->idc = pftmp->idc;
    minidc = pftmp->curcrt->idc;
    if (pftmp->idc == 0) pcrtwrk->pidl = dbgetidl (doc, pftmp);
    else                 pcrtwrk->pidl = dbgetidd (doc, pftmp);

    pftmp = pcrt->fld_first;
    while (1) {
        if (pcrtwrk->firstdark == NULL && pftmp->curcrt->opt.dark)
            pcrtwrk->firstdark = pftmp->curcrt;
        pftmp = pftmp->next;
        pctmp = pftmp->curcrt;
        if (pftmp == NULL || pftmp->opt.first) {
            pcrtwrk->firstmid = pcrtwrk->lastmid = NULL;
            break;
        }
        if (pctmp->idc < pcrt->idc ||
                    !pctmp->opt.visibl && pctmp->idc > pcrt->idc &&
                                  pctmp->fld_last->idf > pcrt->fld_last->idf) {
            pcrtwrk->firstmid = pftmp;
            while (pftmp->next != NULL && !pftmp->next->opt.first)
                pftmp = pftmp->next;
            pcrtwrk->lastmid = pftmp;
            break;
        }
    }

    if (pcrtwrk->firstmid != NULL) {
        minidc = min (minidc, (lastvisi (pcrtwrk->lastmid))->curcrt->idc);
        pftmp = pcrtwrk->lastmid;
        while (1) {
            if (pftmp->opt.visibl || pftmp->curcrt->idc >= minidc) {
                pcrtwrk->lastmid = pftmp;
                break;
            }
            if (pftmp == pcrtwrk->firstmid) {
                pcrtwrk->firstmid = pcrtwrk->lastmid = NULL;
                goto ret;
            }
            pftmp = pftmp->prev;
        }
        pcrtwrk->firstmid->prev->next = pcrtwrk->lastmid->next;
        if (pcrtwrk->lastmid->next != NULL)
            pcrtwrk->lastmid->next->prev = pcrtwrk->firstmid->prev;
    }
ret:
    return pcrtwrk;
}

void dbcrtins (CRTWRK * pcrtwrk) {
    CRT         * pcrt, * pctmp;
    FLD         * pftmp;
    register      i;

    doc = pcrtwrk->doc;
    pcrt = pcrtwrk->crtmain;
    while (pcrt->prev != NULL) pcrt = pcrt->prev;
    pcrtwrk->crtmain = pcrt;

    if (!pcrtwrk->visi) goto lastcor;

    if (pcrt->opt.dark) {
        pcrtwrk->firstdark = pcrt;
        darkinsert (pcrtwrk, pcrtwrk, pcrtwrk->pidl->numdark);
    } else {
        pftmp = pcrt->fld_first;
        if (pcrtwrk->firstdark != NULL) {
            while (!pftmp->curcrt->opt.dark) pftmp = pftmp->next;
            pcrtwrk->firstdark = pftmp->curcrt;
            darkcorrect (pcrtwrk, pcrtwrk->pidl);
        }

        if (pcrtwrk->firstmid != NULL) {
            while (pftmp->next != NULL && !pftmp->next->opt.first)
                pftmp = pftmp->next;
            pcrtwrk->firstmid->prev = pftmp;
            pcrtwrk->lastmid->next = pftmp->next;
            if (pftmp->next != NULL) pftmp->next->prev = pcrtwrk->lastmid;
            pftmp->next = pcrtwrk->firstmid;
        }
    }

    pftmp = pcrt->fld_first;

    instrcorrect (pftmp, pcrtwrk->pidl, pcrtwrk->beginstr);

    pftmp->opt.first = pcrtwrk->optfirst;
    pftmp = firstvisi (pftmp);
    if (pftmp->instr == 1) pftmp->idc = pcrtwrk->idc;

lastcor:
    pftmp = pcrt->fld_first;

    if (pcrt->prevsis == NULL) pcrt->highcrt->lowcrt = pcrt;

    pctmp = pcrt->prevsis;
    while (pctmp != NULL) { pctmp->nextsis = pcrt; pctmp = pctmp->next; }

    pctmp = pcrt->nextsis;
    while (pctmp != NULL) { pctmp->prevsis = pcrt; pctmp = pctmp->next; }

    /*****************************/

    boundcorrect (pcrtwrk, doc->crtroot);

    if (pcrtwrk->visi) {
        i = pcrtwrk->nline - dbcalcln (doc, pftmp, getfldend (pcrt));
        doc->total    -= i;
        doc->lastline -= i;
    }

    free (pcrtwrk);
    return;
}

void   instrcorrect (FLD * pfld, IDLIN * pidl, int instr) {
    register    numdark = pidl->numdark;
    bool        was_dark = NO;

    pfld = firstvisi (pfld);

    do {
        if (pfld->curcrt->opt.dark) { was_dark = YES; numdark--; }
        else if (was_dark) { instr += numdark; was_dark = NO; } /* Перескок */
        pfld->instr = instr++;
        pfld = next (pfld);
    } while (pfld != NULL && pfld->instr != 1);

    return;
}

static void tailcorrect (FLD * first, FLD * last, int places, int idc) {
    register    instr;
    CRT       * pctmp;

    while (1) {
        first->idc = idc;       /* В 1-е поле кортежа прописали */
                                /* владельца шаблона            */
        pctmp = first->curcrt;
        pctmp->fld_first->opt.first = YES;
        for (instr = 1; instr <= places; instr++) {
            if (first->curcrt != pctmp)
                first->curcrt->fld_first->opt.first = NO;
            first->instr  = instr;
            if (first == last) return;
            first = next (first);
        }
    }
}

static void darkinsert (CRTWRK * pcrtwrk, CRTWRK * pdrkwrk, register numdark) {
    CRT         * pcrt = pcrtwrk->firstdark;
    FLD         * pftmp, * pftmp2, * pflast, * pftail;
    register      i, instr = pdrkwrk->beginstr;

    pftmp  = firstvisi (pcrt->fld_first);
    pflast = lastvisi  (getfldend (pcrt));

    while (1) {
        numdark--;
        pftmp->opt.first = NO;
        pftmp->instr = instr++;
        if (pftmp == pflast) break;
        if (numdark == 0) { pftail = next (pftmp); break; }
        pftmp = next (pftmp);
    }
    if (pdrkwrk->firstmid != NULL) {
        pftmp2 = pftmp->curcrt->fld_last;

        pdrkwrk->lastmid->next = pftmp2->next;
        if (pftmp2->next != NULL) pftmp2->next->prev = pdrkwrk->lastmid;

        pdrkwrk->firstmid->prev = pftmp2;
        pftmp2->next = pdrkwrk->firstmid;
    }

    boundcorrect (pdrkwrk, pcrtwrk->crtmain);

    if (pftmp == pflast) return;

    i = pdrkwrk->firstdark->idc;
    tailcorrect (pftail, pflast, doc->idc [i].idd.places, i);
    return;
}

static void darkcorrect (CRTWRK * pcrtwrk, IDLIN * pidl) {
    CRTWRK * pdrkwrk;

    pdrkwrk = crtout (pcrtwrk->firstdark);
    darkinsert (pcrtwrk, pdrkwrk, pidl->numdark);
    free (pdrkwrk);
    return;
}

static FLD * getfldend (CRT * pcrt) {

    while (pcrt->next != NULL) pcrt = pcrt->next;
    return pcrt->fld_end;
}

static bool nextline (CRT * pcrthigh, FLD * endcrtmains) {
    FLD     * first = pcrthigh->fld_first;
    FLD     * last  = pcrthigh->fld_last;

    while (1) {
        if (last == endcrtmains)    return NO;
        if (last == first)          return YES;
        last = last->prev;
    }
}

static void boundcorrect (CRTWRK * pcrtwrk, CRT * pcrtlast) {
    CRT     * crtmain = pcrtwrk->crtmain, * pcrthigh = crtmain->highcrt;
    FLD     * fld_first = crtmain->fld_first, * fld_last = crtmain->fld_last;
    FLD     * endcrtmains;

    if (pcrtlast->opt.dark) return;

    endcrtmains = getfldend (crtmain);

    while (1) {
        if (pcrthigh->fld_first == pcrtwrk->fld_first)
            pcrthigh->fld_first = fld_first;
        if (doc->idc [pcrthigh->idc].idf_last == fld_last->idf)
            pcrthigh->fld_last = fld_last;

        if (!pcrtwrk->visi ||
                    (dbgetidf (doc, pcrthigh->fld_last))->idl ==
                                    (dbgetidf (doc, endcrtmains))->idl) {
            if (pcrthigh->fld_last->idf == fld_last->idf)
                pcrthigh->fld_end = endcrtmains;
            else if (nextline (pcrthigh, endcrtmains))
                pcrthigh->fld_end = endcrtmains;
            else if (pcrtwrk->visi) pcrthigh->fld_end = pcrthigh->fld_last;
        }

        if (pcrthigh->next != NULL)        endcrtmains = getfldend (pcrthigh);
        else if (pcrthigh->prev != NULL)   endcrtmains = pcrthigh->fld_end;

        if (pcrthigh == pcrtlast) return;
        pcrthigh = pcrthigh->highcrt;
    }
}

/*
fld_last - прописывать при "вдергивании"

fld->opt.first  - должен стоять в начале каждой строки
                  если документ невидимый - не выставляется

darkinsert:
             - tailcorrect внести в эту функцию;
*/

