#include <alloc.h>
#include <sldbas.h>

static  int     begidc, begidf;
static  bool    ini;
static  DOC   * doc;
static  CRT  ** begcrt, * pcrtnew, clrroot;
static  FLD  ** begfld;
static  IDCRT * idc;
static  IDFLD * idf;

void            instrcorrect (FLD * pfld, IDLIN * pidl, int instr);

static void     __crtdel     (CRT * high);
static void     __flddel     (CRT * crtmain);
static FLD   ** __fldmake    (int lastidf);
static int      __crtmake    (CRT * high, int first, int last);
static void     __iniwrap    (DOC * doc, CRT * pcrt);

bool dbmkroot (DOC * doc, bool init) {

    ini = init;
    if ((doc->crtroot = _crtmake (doc, &clrroot)) == NULL) return NO;

    if (doc->opt.visibl) {
        doc->lastline = dbgetpno (doc, lastvisi (doc->crtroot->fld_end));
        doc->total    = doc->o.total - 1;
    }
    doc->curfld = doc->crtroot->fld_first;
    __iniwrap (doc, doc->crtroot);
    return YES;
}

FLD * dbcrtdel (DOC * pdoc, CRT * crtmain) {
    FLD     * pfldret = NULL;
    CRTWRK  * pcrtwrk;

    doc = pdoc;

    if (crtmain == doc->crtroot) { doc->crtroot = NULL; _crtdel (crtmain); }
    else {
        pcrtwrk = dbcrtout (doc, crtmain);
        pfldret = dbdelout (doc, crtmain, pcrtwrk);
        dbcrtins (pcrtwrk);
    }
    return pfldret;
}

FLD * dbdelout (DOC * pdoc, CRT * crtmain, CRTWRK * pcrtwrk) {
    FLD * pfldret;

    doc = pdoc;

    if (crtmain->prev == NULL && crtmain->next == NULL) return NULL;
    if (crtmain->next != NULL) pcrtwrk->crtmain = crtmain->next;
    else                       pcrtwrk->crtmain = crtmain->prev;
    pfldret = pcrtwrk->crtmain->fld_first;
    _outcrt (crtmain);
    _crtdel (crtmain);
    return pfldret;
}

FLD * dbcrtmul (DOC * doc, CRT * crtmain, bool down, bool init) {
    FLD     * pfldret;
    CRTWRK  * pcrtwrk;

    pcrtwrk = dbcrtout (doc, crtmain);
    pfldret = dbmulout (doc, crtmain, pcrtwrk, down, init);
    dbcrtins (pcrtwrk);

    return pfldret;
}

FLD * dbmulout (DOC *doc, CRT *crtmain, CRTWRK *pcrtwrk, bool down, bool init) {
    FLD     * pfldret;
    CRT     * crtnew;

    ini = init;
    if ((crtnew = _crtmake (doc, crtmain)) == NULL) return NULL;
    _inscrt (crtnew, crtmain, down);
    pfldret = crtnew->fld_first;
    if (pcrtwrk->visi) {                                      /*?????*/
        instrcorrect (pfldret, &idc [begidc].idd, 1);
        if (crtnew->opt.dark) {
            pfldret->opt.first = NO;
            (firstvisi (pfldret))->idc = 0;
        } else {
            pfldret->opt.first = YES;
            (firstvisi (pfldret))->idc = begidc;
        }
    }
    __iniwrap (doc, crtnew);
    return pfldret;
}

void _crtdel (CRT * crtmain) {

    __flddel (crtmain);
    __crtdel (crtmain);
    free    (crtmain);
    return;
}

void _outcrt (CRT * crtmain) {                  /** Выдирает crt из связки **/
    FLD     * first = crtmain->fld_first;
    FLD     * last  = crtmain->fld_end;

    if (crtmain->prev != NULL) crtmain->prev->next = crtmain->next;
    if (crtmain->next != NULL) crtmain->next->prev = crtmain->prev;

    if (first->prev != NULL) first->prev->next = last->next;
    if (last->next  != NULL) last->next->prev  = first->prev;

    return;
}

                                           /** Вставляет голый crt в связку **/
void _inscrt (CRT * crtnew, CRT * crtover, bool down) {
    FLD * newfirst  = crtnew->fld_first;
    FLD * newlast   = crtnew->fld_end;
    FLD * overfirst = crtover->fld_first;
    FLD * overlast  = crtover->fld_end;

    if (down) {
        crtnew->next   = crtover->next;
        crtnew->prev   = crtover;

        newfirst->prev = overlast;
        newlast->next  = overlast->next;
    } else {
        crtnew->next   = crtover;
        crtnew->prev   = crtover->prev;

        newfirst->prev = overfirst->prev;
        newlast->next  = overfirst;
    }
    if (crtnew->prev   != NULL) crtnew->prev->next   = crtnew;
    if (crtnew->next   != NULL) crtnew->next->prev   = crtnew;

    if (newfirst->prev != NULL) newfirst->prev->next = newfirst;
    if (newlast->next  != NULL) newlast->next->prev  = newlast;

    return;
}

CRT * _crtmake (DOC * doc, CRT * crtmain) { /** Делает голый кортеж **/
    FLD      ** ppfldnew;
    CRT      ** ppcrtnew;
    int         numcrt;
    register    lastidc, i;

    idc = doc->idc;
    idf = doc->idf;

    begidc  = crtmain->idc;
    begidf  = idc [begidc].idf_first;
    lastidc = idc [begidc].idc_last;

    if (lastidc == -1)  numcrt = 1;
    else                numcrt = lastidc - begidc + 1;

    if ((ppcrtnew = calloc (numcrt, sizeof(CRT *))) == NULL) return NULL;
    for (i = 0; i < numcrt; i++)
        if ((ppcrtnew [i] = utalloc (CRT)) == NULL) goto ret;

    begcrt  = ppcrtnew - begidc;
    pcrtnew = ppcrtnew [0];

    if ((ppfldnew = __fldmake (idc [begidc].idf_last)) == NULL) goto ret;

    if (lastidc > 0)
        __crtmake (ppcrtnew [0], begidc + 1, lastidc); /* pcrtnew имеет дочек */

    pcrtnew->highcrt = crtmain->highcrt;
    pcrtnew->nextsis = crtmain->nextsis;
    pcrtnew->prevsis = crtmain->prevsis;
    pcrtnew->idc     = begidc;
    pcrtnew->opt     = idc [begidc].opt;


    free (ppcrtnew);
    free (ppfldnew);
    return pcrtnew;
ret:
    for (i = 0; i < numcrt; i++) free (ppcrtnew [i]);
    free (ppcrtnew);
    return NULL;
}

/******************* Внутренние функции *********************/

static int __crtmake (CRT * high, int first, int last) {
    register  cur  = first, m;
    int       n, lastsis;
    CRT     * pc = begcrt [first];

    high->lowcrt = pc;

    for (lastsis = cur; cur <= last; cur++) {
        pc = begcrt [cur];
        pc->idc     = cur;
        pc->highcrt = high;
        (begcrt [lastsis])->nextsis = pc;
        pc->prevsis = begcrt [lastsis];
        lastsis = cur;
        pc->opt = idc [cur].opt;

        m = idc [cur].idf_first;
        n = idc [cur].idf_last;

        pc->fld_first = begfld [m];
        pc->fld_last  = pc->fld_end = begfld [n];

        for (; m <= n; m++) (begfld [m])->curcrt = pc;

        if (idc [cur].idc_last > 0)                  /* pc имеет дочек */
            cur = __crtmake (pc, cur + 1, idc [cur].idc_last);
    }
    (begcrt [lastsis])->nextsis = (begcrt [first])->prevsis = NULL;
    return (cur - 1);
}

static FLD  ** __fldmake (int lastidf) {
    register    curidf = begidf, i;
    FLD       * p, ** ppfldnew;
    int         numfld = lastidf - begidf + 1;



    if ((ppfldnew = calloc (numfld, sizeof (FLD *))) == NULL) return NULL;

    for (i = 0; i < numfld; i++)
        if ((ppfldnew [i] = utalloc (FLD)) == NULL) goto ret;

    begfld = ppfldnew - begidf;


    pcrtnew->fld_first = ppfldnew [0];
    pcrtnew->fld_last  = pcrtnew->fld_end = begfld [lastidf];


    for (; curidf <= lastidf; curidf++) {
        p = begfld [curidf];
        p->prev   = begfld [curidf - 1];
        p->next   = begfld [curidf + 1];
        p->idf    = curidf;
        p->opt    = idf [curidf].opt;
        p->prnopt = idf [curidf].prnopt;
        p->instr  = idf [curidf].instr;
        p->idn    = idf [curidf].idn;
        if (ini) {
            p->val = idf [curidf].inival;
            if (idf [curidf].valtype == SL_TEXT) {
                if (p->val.s.len > 0) {
                    if ((p->val.s.p = malloc (p->val.s.len + 1)) == NULL) goto ret;
                    utmovabs (p->val.s.p, idf [curidf].inival.s.p,
                                                            p->val.s.len + 1, 0);
                } else  p->val.s.p = NULL;
            } else if (idf [curidf].valtype == SL_DATE &&
                                    (idf [curidf].valopt.date.opt & CV_TODAY))
                p->val.l = utdtoday ();
        }
        p->curcrt = pcrtnew;
    }
    (begfld [begidf])->prev = (begfld [lastidf])->next = NULL;

    return ppfldnew;
ret:
    if (ini) for (curidf--; curidf >= begidf; curidf--)
        if (idf [curidf].valtype == SL_TEXT && p->val.s.len > 0)
            utfree (&begfld [curidf]->val.s.p);
    for (i = 0; i < numfld; i++) free (ppfldnew [i]);
    free (ppfldnew);
    return NULL;
}

static void __flddel (CRT * crtmain) {
    FLD       * first = crtmain->fld_first;
    FLD       * last  = crtmain->fld_end;

    while (1) {
        if ((dbgetidf (doc, first))->valtype == SL_TEXT ||
                                                        doc->doctype == DB_QRY)
            utfree (&first->val.s.p);
        if (first == last) break;
        first = first->next;
        free (first->prev);
    }
    free  (last);
    return;
}

static void __crtdel (CRT * high) {
    CRT     * pctmp, * p;

    pctmp = high->lowcrt;
    if (pctmp == NULL) return;
    while (1) {
        p = pctmp->next;
        if (p != NULL)  {
            while (1) {
                __crtdel (p);
                if (p->next != NULL) { p = p->next;  utfree (&p->prev); }
                else                 { free (p);     break;             }
            }
        }
        __crtdel (pctmp);
        if (pctmp->nextsis != NULL) {
            pctmp = pctmp->nextsis;
            utfree (&pctmp->prevsis);
        } else { free (pctmp); return; }
    }
}

static void  __iniwrap (DOC * doc, CRT * pcrt) {
    FLD         * pfldcur  = pcrt->fld_first;
    FLD         * pfldlast = pcrt->fld_last;
    IDCOL       * curcol;
    TXTVAL      * curval;
    byte        * pbeg, * pend;
    register      i;
    bool          first;

    if (!ini) return;
    while (1) {
        if (pfldcur->opt.wrap && !pfldcur->opt.done) {
            first = YES;
            dboutcol (doc, pfldcur, NO);
            curcol = firstcol;
            do {
                curval = curcol->pval;
                if ((i = curval->val.s.blank) == 0) break;
                pbeg = curval->val.s.p;
                while (1) {
                    (char *) pend = strchr ((char *) pbeg, '\n');
                    curval->val.s.len = (int) (pend - pbeg);
                    if (first) first = NO;
                    else {
                        curval->val.s.p = malloc (curval->val.s.len + 1);
                        utmovabs (curval->val.s.p, pbeg, curval->val.s.len, 0);
                    }
                    curval->val.s.p [curval->val.s.len] = EOS;
                    i--;
                    if (i == 0) break;
                    curval->next = utalloc (TXTVAL);
                    curval->next->prev = curval;
                    curval = curval->next;
                    curval->prnopt = curval->prev->prnopt;
                    curval->par = YES;
                    pbeg = pend + 1;
                }
                curval->next = curcol->pval;
                curcol->pval->prev = curval;
                curcol = curcol->next;
            } while (curcol != NULL);
            dbfmtcol (NO);
        }
        if (pfldcur == pfldlast) break;
        pfldcur = pfldcur->next;
    }

    pfldcur  = pcrt->fld_first;
    while (1) {
        pfldcur->opt.done = pfldcur->opt.change = NO;
        if (pfldcur == pfldlast) break;
        pfldcur = pfldcur->next;
    }

    return;
}

