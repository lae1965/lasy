#include <sldbas.h>

static  DOC       * docfrom;
static  DOC       * docto;
static IDCOL      * curcol;
static  bool        movpropt;

static void  bval2lval    (CRT * bpapa, CRT * lpapa, FLD * bfld, FLD * lfld);
static bool  crt2crt      (CRT * bcrt, CRT * lcrt, bool top);

bool  dbcrtocr (DOC * pdocto, DOC * pdocfrom,
                                CRT * pcrtto, CRT * pcrtfrom, bool _movpropt) {
    FLD     * curfldto = pdocto->curfld;
    bool      retcode;

    pdocto->curfld = NULL;
    docfrom  = pdocfrom;
    docto    = pdocto;
    movpropt = _movpropt;
    retcode = crt2crt (pcrtfrom, pcrtto, YES);
    pdocto->curfld = curfldto;
    dbclrdone (pcrtto);
    return retcode;
}

static bool crt2crt (CRT * papacrtfrom, CRT * pcrtto, bool top) {
    FLD     * curfldfrom, * lastfldfrom;
    FLD     * curfldto, * lastfldto;
    CRT     * pcrtfrom, * curcrt;
    char    * pname;

beg:
    if (pcrtto->opt.multy && pcrtto->fld_first->opt.wrap) goto sys;
    if (top) pcrtfrom = papacrtfrom;
    else pcrtfrom = get1stfrom (docfrom, papacrtfrom,
                                    (char *) docto->cnam [pcrtto->idc].nam);
    if (pcrtfrom == NULL) goto sys;

    while (1) {    /****************** по кортежам ********************/
        curfldto  = pcrtto->fld_first;
        lastfldto = pcrtto->fld_last;

        while (1) {   /********************* по полям ********************/
            if (curfldto->opt.done) goto nextfld;
            curcrt = curfldto->curcrt;
            if (curcrt->opt.multy && curfldto->opt.wrap)
                curcrt = curcrt->highcrt;
            if (curcrt == pcrtto) {
                curfldfrom  = pcrtfrom->fld_first;
                lastfldfrom = pcrtfrom->fld_last;
                pname = (char *) docto->fnam [curfldto->idn].nam;
                while (strcmp ((char *) docfrom->fnam [curfldfrom->idn].nam,
                                                                 pname) != 0) {
                    if (curfldfrom == lastfldfrom) {
                        if (curfldto->opt.wrap) {
                            if (curcol == NULL) {
                                dboutcol (docto, curfldto, NO);
                                curcol = firstcol;
                            }
                            curcol = curcol->next;
                            if (curcol == NULL) dbfmtcol (NO);
                        }
                        goto nextfld;
                    }
                    curfldfrom = curfldfrom->next;
                }
                bval2lval (pcrtfrom, pcrtto, curfldfrom, curfldto);
            }
nextfld:
            if (curfldto == lastfldto) break;
            curfldto = curfldto->next;
        }
        if (pcrtto->lowcrt  != NULL)
            if (!crt2crt (pcrtfrom, pcrtto->lowcrt, NO)) return NO;

        if (!top && (pcrtfrom = getnextfrom (pcrtfrom, papacrtfrom->fld_end))
                                                                     != NULL) {
            curfldto = dbcrtmul (docto, pcrtto, YES, NO);
            pcrtto = pcrtto->next;
            continue;
        }
sys:
        if (!top && pcrtto->nextsis != NULL) {
            pcrtto = pcrtto->nextsis;
            goto beg;
        }
        return YES;
    }
}

static void bval2lval (CRT *papafrom, CRT *papato, FLD *fldfrom, FLD *fldto) {
    IDFLD        * pidf;
    TXTVAL       * ptxtval;
    FLD          * pftmp;
    int            len = 0, valtypefrom, valtypeto;
    bool           prevpar;

    pidf = dbgetidf (docfrom, fldfrom);
    valtypefrom = pidf->valtype;
    valtypeto   = (dbgetidf (docto, fldto))->valtype;
    if (valtypefrom == SL_CDF) valtypefrom = SL_INT;
    if (valtypeto   == SL_CDF) valtypeto   = SL_INT;
    if (valtypefrom != valtypeto) return;
    switch (valtypefrom) {
        case SL_TEXT:
            if (fldto->opt.wrap) {
                if (curcol == NULL) {
                    dboutcol (docto, fldto, NO);
                    curcol = firstcol;
                }
                pftmp = curcol->first;
                do    pftmp->opt.done = YES;
                while ((pftmp = nextcont (pftmp, papato)) != NULL);
                /* перенос val'ов из fldfrom'ов в txtval'ы */
                ptxtval = curcol->pval;
                while (1) {
                    ptxtval->par = fldfrom->opt.par;
                    len  = ptxtval->val.s.len = fldfrom->val.s.len;
                    if (len > 0) {
                        ptxtval->val.s.p = calloc (len + 1, 1);
                        utmovabs (ptxtval->val.s.p, fldfrom->val.s.p, len, 0);
                    }
                    if (movpropt) ptxtval->prnopt      = fldfrom->prnopt;
                    else          ptxtval->prnopt.full = fldfrom->prnopt.full;
                    if ((fldfrom = nextcont (fldfrom, papafrom)) == NULL) break;
                    prevpar = ptxtval->par;
                    ptxtval = valbreak (ptxtval, NULL, 0);
                    ptxtval->prev->par = prevpar;
                }
                curcol = curcol->next;
                if (curcol == NULL) dbfmtcol (NO);
                return;
            }
            len = fldto->val.s.len = fldfrom->val.s.len;
            if (len != 0) {
                fldto->val.s.p = calloc (len + 1, 1);
                utmovabs (fldto->val.s.p, fldfrom->val.s.p, len, 0);
            }
            break;
        default :    fldto->val = fldfrom->val;    break;
    }
    if (movpropt) fldto->prnopt      = fldfrom->prnopt;
    else          fldto->prnopt.full = fldfrom->prnopt.full;
    fldto->opt.done = YES;
    return;
}

