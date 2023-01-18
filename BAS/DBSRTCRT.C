#include <sldbas.h>

static int     idf, fwd;
static int   * pidn, numidn;
static DOC   * doc;

static int    cmpval  (void * pc1, void * pc2);
static int    _cmpval (CRT * pc1, CRT * pc2);
static FLD  * fndfld  (CRT * pcrt);
static void * getpar  (FLD ** ppfld, CRT * papa);

FLD * dbsrtcrt (DOC * pdoc, FLD * pfld, bool forward) {
    CRTWRK  * pcrtwrk;
    CRT     * pcrt = pfld->curcrt, * pcfirst;
    CRT    ** ppcrt;
    FLD     * fld_prev, * fld_next;
    int       ncrt = 1, i;

    if (pfld->opt.wrap) {
         if (pcrt->opt.multy) pcrt = pcrt->highcrt;
         while (1) {
            fld_prev = prevcont (pfld, pcrt);
            if (fld_prev == NULL) break;
            pfld = fld_prev;
         }
    }
    doc = pdoc;
    idf = pfld->idf;
    pidn = (int *) Db_pwrk;
    pidn [0] = pfld->idn;
    if (!forward) fwd = -1;
    else          fwd =  1;
    while (pcrt->prev != NULL) pcrt = pcrt->prev;
    pcfirst = pcrt;
    fld_prev = pcfirst->fld_first->prev;
    while (pcrt->next != NULL) { pcrt = pcrt->next; ncrt++; }

    ppcrt = malloc (ncrt * sizeof (CRT *));
    for (pcrt = pcfirst, i = 0; i < ncrt; i++, pcrt = pcrt->next)
        ppcrt [i] = pcrt;

    pcrtwrk = dbcrtout (doc, pcfirst);
    fld_prev = ppcrt [0]->fld_first->prev;
    fld_next = ppcrt [ncrt - 1]->fld_end->next;

    utqsort (ppcrt, ncrt, sizeof (CRT *), cmpval);

    for (i = 0; i < ncrt; i++) {
        ppcrt [i]->prev = ppcrt [i - 1];
        ppcrt [i]->next = ppcrt [i + 1];
        ppcrt [i]->fld_first->prev = ppcrt [i - 1]->fld_end;
        ppcrt [i]->fld_end->next   = ppcrt [i + 1]->fld_first;
    }
    ppcrt [0]->prev = ppcrt [ncrt - 1]->next = NULL;
    ppcrt [0]->fld_first->prev = fld_prev;
    if (fld_prev != NULL) fld_prev->next = ppcrt [0]->fld_first;
    ppcrt [ncrt - 1]->fld_end->next = fld_next;
    if (fld_next != NULL) fld_next->prev = ppcrt [ncrt - 1]->fld_end;

    dbcrtins (pcrtwrk);
    fld_prev = fndfld (ppcrt [0]);
    free (ppcrt);
    return fld_prev;
}

static int   cmpval (void * ppc1, void * ppc2) {
    FLD    * pf1;
    CRT    * pc1, * pc2, * pcrt;
    register i, savidf = idf;

    numidn = 1;
    pc1 = (CRT *) (* (CRT **) ppc1);
    pc2 = (CRT *) (* (CRT **) ppc2);
    i = _cmpval (pc1, pc2);
    if (i != 0) return i;
    pf1 = pc1->fld_first;
    while (1) {
        pcrt = pf1->curcrt;
        if (pf1->opt.wrap && pcrt->opt.multy) pcrt = pcrt->highcrt;
        if (pcrt == pc1) {
            for (i = 0; i < numidn; i++) {
                if (pidn [i] == pf1->idn) goto lab;
            }
            idf = pf1->idf;
            if ((i = _cmpval (pc1, pc2)) != 0) goto ret;
            numidn++;
            pidn [numidn] = pf1->idn;
        }
lab:
        if (pf1 == pc1->fld_last) break;
        pf1 = pf1->next;
    }
ret:
    idf = savidf;
    return i;
}

static int   _cmpval (CRT * pc1, CRT * pc2) {
    FLD    * pf1, * pf2;
    void   * p1, *p2;
    double   result = 0.0;
    IDFLD  * pidf;
    register i = 1;

    pf1 = fndfld (pc1);
    pf2 = fndfld (pc2);

    pidf = dbgetidf (doc, pf1);
    switch (pidf->valtype) {
        case SL_TEXT:
            while (1) {
                p1 = getpar (&pf1, pc1);
                p2 = getpar (&pf2, pc2);
                if (p1 == NULL && p2 == NULL) { i = 0; goto lab; }
                if (p1 == NULL) { i = -1; free (p2); goto ret; }
                if (p2 == NULL) { i =  1; free (p1); goto ret; }
                i = strcmp (p1, p2);
                free (p1);  free (p2);
                if (i != 0) goto ret;
lab:
                if (pf1 == NULL && pf2 == NULL) goto ret;
                if (pf1 == NULL) { i = -1; goto ret; }
                if (pf2 == NULL) { i =  1; goto ret; }
            }
        case SL_CDF:
            p1 = p2 = NULL;
            if (pf1->prnopt.full)
                p1 = (dbcdf2nam (pidf->fldlink.link.pdoc, pf1->val.i))->s.p;
            if (pf2->prnopt.full)
                p2 = (dbcdf2nam (pidf->fldlink.link.pdoc, pf2->val.i))->s.p;
            if (p1 == NULL && p2 == NULL) { i = 0; goto ret; }
            if (p1 == NULL) { i = -1; goto ret; }
            if (p2 == NULL) { i =  1; goto ret; }
            i = strcmp (p1, p2);
            goto ret;
        case SL_INT:
            result = (double) (pf1->val.i - pf2->val.i);
            break;
        case SL_LONG:
        case SL_DATE:
            result = (double) (pf1->val.l - pf2->val.l);
            break;
        case SL_FLOAT:
            result = (double) (pf1->val.f - pf2->val.f);
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            result = pf1->val.d - pf2->val.d;
            break;
        default :
            if (Db_Fn_cmpval != NULL)
                result = (* Db_Fn_cmpval) (&pf1->val, &pf2->val, pidf->valtype);
            break;
    }
    if (result == 0.0) return 0;
    if (result < 0.0) i = -1;
ret:
    i *= fwd;
    return i;
}

static FLD * fndfld (CRT * pcrt) {
    FLD * pfld = pcrt->fld_first;

    while (pfld->idf != idf) pfld = pfld->next;
    return pfld;
}

static void * getpar (FLD ** ppfld, CRT * papa) {
    FLD     * pftmp = * ppfld;
    char    * p = NULL, * q;
    int       vallen;
    register  len = 0, n = 0;

    while (1) {
        len += pftmp->val.s.len;
        n++;
        pftmp = nextcont (pftmp, papa);
        if (pftmp == NULL || pftmp->opt.par) break;
    }
    if ( len == 0) goto ret;
    len += n;
    if ((p = malloc (len)) == NULL) goto ret;

    for (pftmp = * ppfld, q = p; n > 0; n--, pftmp = nextcont (pftmp, papa)) {
        vallen = pftmp->val.s.len;
        utmovabs (q, pftmp->val.s.p, vallen, 0);
        q [vallen] = ' ';
        q += vallen + 1;
    }
    p [len - 1] = EOS;

ret:
    * ppfld = pftmp;
    return p;
}

