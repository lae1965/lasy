/******************************************************************************/
/*   dbtstrec () - Æ·„È•·‚¢´Ô•‚ Ø‡Æ¢•‡™„ Æ§≠Æ© ß†Ø®·® ≠† ·ÆÆ‚¢•‚·‚¢®• ß†Ø‡Æ·„ */
/*                                                                            */
/*   bool dbtstrec (DOC * pbasedoc, DOC * pqrydoc);                           */
/*                                                                            */
/*   DOC * pbasedoc - °†ßÆ¢Î©   §Æ™„¨•≠‚;                                     */
/*   DOC * pqrydoc  - ß†Ø‡Æ·≠Î© §Æ™„¨•≠‚;                                     */
/*                                                                            */
/*   ÇÆß¢‡†È†•‚:      YES - ß†Ø®·Ï ·ÆÆ‚¢•‚·‚¢„•‚ ß†Ø‡Æ·„;                     */
/*                    NO  - ß†Ø®·Ï ≠• ·ÆÆ‚¢•‚·‚¢„•‚ ß†Ø‡Æ·„;                  */
/******************************************************************************/

#include   <sldbas.h>
#include   <alloc.h>

typedef struct fnd {
    FLD        * pfld;
    struct fnd * next;
    struct frg * pfrg;
    bool         neg;
    int          code;
} FND;

typedef struct frg {
    FND        * first;
    struct frg * next;
    byte         nfrg;
    byte         ngrid;
} FRG;

CRT * getnextfrom (CRT * pcrtfrom, FLD * plastfld);
CRT * get1stfrom  (DOC * basedoc, CRT * bpapacrt, char * pnam);

static bool     tstcrt      (CRT * bpapacrt, CRT * qcrt, bool top);
static bool     fndfld      (CRT * pcrtfrom, CRT * qcrt);
static bool     tstfld      (CRT * bpapacrt, FLD * qryfld, FLD * bcurfld);
static bool     tstval      (FRG * pfrg, FLD * bcurfld);
static bool     cmpval      (FND * pfnd, FLD * bfld);
static bool     vfygrid     (byte * pgrid, int nq, int nb);
static void     clrfrg      (FRG * pfrg);
static bool     q2qry       (CRT * qcrt, CRT * qrycrt);
static bool     qval2qry    (FLD * qfld, FLD * qryfld);
static FRG    * makefrg     (void);
static void     clrfrg      (FRG * pfrg);

static DOC  * qrydoc, * basedoc, * qdoc;
static FLD  * pfndfld;
static byte * psos;

bool dbtstrec (DOC * pbasedoc, DOC * pqrydoc, CRT * bcrt, CRT * qcrt) {

    basedoc = pbasedoc;
    qrydoc  = pqrydoc;
    return (tstcrt (bcrt, qcrt, YES));
}

static bool tstcrt (CRT * bpapacrt, CRT * qcrt, bool top) {
    FLD     * papalastfld;
    CRT     * pcrtfrom, * p1stcrtfrom;
    byte    * pgrid;
    int       ncrtqry, ncrtbase, Y;
    register  i, j;
    bool      done;

    papalastfld = bpapacrt->fld_end;

beg:
    if (qcrt->opt.multy) goto sys;
    if (top) p1stcrtfrom = bpapacrt;
    else     p1stcrtfrom = get1stfrom (basedoc, bpapacrt,
                                    (char *) qrydoc->cnam [qcrt->idc].nam);

    if (p1stcrtfrom == NULL) goto sys;

    /****** èÆ§·Á•‚ ncrtqry ® ncrtbase ******/
    for (ncrtqry = 0, pcrtfrom = qcrt; pcrtfrom != NULL; ncrtqry++)
        pcrtfrom = pcrtfrom->next;

    if (!top) {
        for (ncrtbase = 0, pcrtfrom = p1stcrtfrom; pcrtfrom != NULL; ncrtbase++)
            pcrtfrom = getnextfrom (pcrtfrom, papalastfld);
    } else ncrtbase = 1;

    pgrid = calloc ((ncrtbase + 1) * (ncrtqry + 1), 1);

    i = 1;
    while (1) {

        for (j = 1, pcrtfrom = p1stcrtfrom; j <= ncrtbase; j++) {
            if ((done = fndfld (pcrtfrom, qcrt)) == YES) {
                if (qcrt->lowcrt  != NULL)
                    done = tstcrt (pcrtfrom, qcrt->lowcrt, NO);
                if (done) {
                    Y = (ncrtqry + 1) * j;
                    pgrid [Y + i] = 1;
                    pgrid [Y]++;
                    pgrid [i]++;
                }
            }

            pcrtfrom = getnextfrom (pcrtfrom, papalastfld);
        }
        i++;
        if (i > ncrtqry) break;
        qcrt = qcrt->next;
    }

    done = vfygrid (pgrid, ncrtqry, ncrtbase);
    free (pgrid);
    if (!done) return NO;

sys:
    if (qcrt->nextsis != NULL) {
        qcrt = qcrt->nextsis;
        goto beg;
    }
    return YES;
}

static bool fndfld (CRT * pcrtfrom, CRT * qcrt) {
    FLD     * bcurfld;
    FLD     * qryfld   = qcrt->fld_first;
    char    * pname;

    while (1) {
        if (qryfld->curcrt == qcrt && qryfld->prnopt.full) {
            bcurfld = pcrtfrom->fld_first;
            pname = (char *) qrydoc->fnam [qryfld->next->idn].nam;
            while (strcmp ((char *) basedoc->fnam [bcurfld->idn].nam,
                                                                pname) != 0) {
                if (bcurfld == pcrtfrom->fld_end) goto lab;
                bcurfld = bcurfld->next;
            }
            /* è‡Æ¢•‡™† ØÆ´•© ß†Ø®·® ® ß†Ø‡Æ·† ≠† ·ÆÆ‚¢•‚·‚¢®• */
            if (!tstfld (pcrtfrom, qryfld, bcurfld)) return NO;
        }
lab:
        if ((qryfld = nextcont (qryfld, qcrt)) == NULL) return YES;
    }
}

static bool tstfld (CRT * bpapacrt, FLD * qryfld, FLD * bcurfld) {
    FRG     * pfrg = (FRG *) qryfld->val.q;
    FND     * pfnd;
    FLD     * savbcur = bcurfld;
    byte    * pgrid;
    int       Y, nbfld, k;
    register  i, j;
    bool      result;

    for (nbfld = 0; bcurfld != NULL; nbfld++)
        bcurfld = nextcont (bcurfld, bpapacrt);

    while (1) {
        pfnd = pfrg->first;
        pgrid = calloc ((nbfld + 1) * (pfrg->ngrid + 1), 1);

        for (k = 0, i = 1; i <= pfrg->nfrg; i++, pfnd = pfnd->next) {

            bcurfld = savbcur;
            if (pfnd->code != 1) k++;

            for (j = 1; j <= nbfld; j++) {

                if (pfnd->pfrg == NULL) result = cmpval (pfnd, bcurfld);
                else                    result = tstval (pfnd->pfrg, bcurfld);

                if (pfnd->neg) result = !result;

                if (pfnd->code == 1) {
                    if (!result) goto lab;
                } else {
                    if (result) {
                        Y = (pfrg->nfrg + 1) * j;
                        pgrid [Y + k] = 1;
                        pgrid [Y]++;
                        pgrid [k]++;
                    }
                }
                bcurfld = nextcont (bcurfld, bpapacrt);
            }

        }

        result = vfygrid (pgrid, pfrg->ngrid, nbfld);
lab:
        free (pgrid);

        if (result)        return YES;
        if (pfrg->next == NULL) return NO;
        pfrg = pfrg->next;
    }
}

static bool tstval (FRG * pfrg, FLD * bcurfld) {
    FND     * pfnd;
    bool      result;
    register  i;


    while (1) {
        pfnd = pfrg->first;

        for (i = 1; i <= pfrg->nfrg; i++, pfnd = pfnd->next) {

            if (pfnd->pfrg == NULL)  result = cmpval (pfnd, bcurfld);
            else                     result = tstval (pfnd->pfrg, bcurfld);

            if (pfnd->neg) result = !result;
            if (!result) break;
        }

        if (result)             return YES;
        if (pfrg->next == NULL) return NO;
        pfrg = pfrg->next;
    }
}

static bool cmpval (FND * pfnd, FLD * bfld) {
    VAL      qval = pfnd->pfld->val, bval = bfld->val;
    double   result;
    IDFLD  * pidf;
    bool     success = NO;

    if (pfnd->pfld == NULL) return (!bfld->prnopt.full);
    if (!bfld->prnopt.full) return NO;

    pidf = dbgetidf (basedoc, bfld);
    switch (pidf->valtype) {
        case SL_TEXT:
            if (* qval.s.p == '*') return dbfndfrg (bval.s.p, qval.s.p + 1);
            result = (double) dbcmpsmp (bval.s.p, qval.s.p);
            break;
        case SL_CDF:
        case SL_INT:
            result = (double) (bval.i - qval.i);
            break;
        case SL_LONG:
        case SL_DATE:
            result = (double) (bval.l - qval.l);
            break;
        case SL_FLOAT:
            result = (double) (bval.f - qval.f);
            break;
        case SL_MONEY:
            result = bval.d - qval.d;
            if (result < 0.005 && result > -0.005) result = 0.0;
            break;
        case SL_DOUBLE:
            result = bval.d - qval.d;
            break;
        default :
            if (Db_Fn_cmpval != NULL)
                result = (* Db_Fn_cmpval) (&bval, &qval, pidf->valtype);
            break;
    }

    if (result >  0.0 && (pfnd->code & 8) ||
        result <  0.0 && (pfnd->code & 4) ||
        result == 0.0 && (pfnd->code & 2))  success = YES;

    return success;
}

CRT * get1stfrom (DOC * basedoc, CRT * bpapacrt, char * pnam) {
    FLD     * pcurfld  = bpapacrt->fld_first;
    FLD     * plastfld = bpapacrt->fld_last;
    CRT     * curcrt;

    while (1) {
        curcrt = pcurfld->curcrt;
        if (strcmp (pnam, (char *) basedoc->cnam [curcrt->idc].nam) == 0)
            return curcrt;
        if (curcrt->opt.multy) {
            curcrt = curcrt->highcrt;
            if (strcmp (pnam, (char *) basedoc->cnam [curcrt->idc].nam) == 0)
                return curcrt;
        }
        if (pcurfld == plastfld) return NULL;
        pcurfld = pcurfld->next;
    }
}

CRT * getnextfrom (CRT * pcrtfrom, FLD  * plastfld) {
    FLD     * pcurfld = pcrtfrom->fld_end;
    CRT     * curcrt;
    int       idc = pcrtfrom->idc;


    if (pcrtfrom->next != NULL) return pcrtfrom->next;
    if (pcurfld->next == NULL) return NULL;

    while (pcurfld != plastfld) {
        pcurfld = pcurfld->next;
        curcrt = pcurfld->curcrt;
        if (curcrt->idc == idc) return curcrt;
        if (curcrt->opt.multy) {
            curcrt = curcrt->highcrt;
            if (curcrt->idc == idc) return curcrt;
        }
    }
    return NULL;
}

static bool vfygrid (byte * pgrid, int nq, int nb) {
    register   i, j;
    int        Y, X;
    byte       nx;

    if (nq == 0) return YES;
    for (j = 1; j <= nb; j++) {
        Y = (nq + 1) * j;
        nx = pgrid [Y];
        if (nx == 0) continue;
        for (; nx > 1; nx--) {
            for (i = 1; pgrid [i + Y] == 0; i++);
            X = i;
            for (i++; i <= nq; i++)
                if (pgrid [i + Y] == 1 && pgrid [i] > pgrid [X]) X = i;
            pgrid [X + Y] = 0;
            pgrid [X]--;
        }
    }
    for (i = 1; i <= nq; i++)
        if (pgrid [i] == 0) return NO;

    return YES;
}

bool dbq2qry (DOC * pqdoc, DOC * pqrydoc) {

    qrydoc = pqrydoc;
    qdoc   = pqdoc;
    return (q2qry (pqdoc->crtroot, pqrydoc->crtroot));
}

static bool q2qry (CRT * qcrt, CRT * qrycrt) {
    FLD     * qfld;
    FLD     * qryfld;

    while (1) {
        qfld     = qcrt->fld_first;
        if (qcrt->opt.multy) goto sis;
        qryfld   = qrycrt->fld_first;

        while (1) {
            if (qfld->curcrt == qcrt && !qval2qry (qfld, qryfld)) return NO;
            if ((qfld = nextcont (qfld, qcrt)) == NULL) break;
            qryfld = nextcont (qryfld, qrycrt);
        }

        if (qcrt->lowcrt != NULL)
            if (!q2qry (qcrt->lowcrt, qrycrt->lowcrt)) return NO;

        if (qcrt->next != NULL) {
            dbcrtmul (qrydoc, qrycrt, YES, NO);
            qcrt   = qcrt->next;
            qrycrt = qrycrt->next;
            continue;
        }
sis:
        if (qcrt->nextsis != NULL) {
            qcrt   = qcrt->nextsis;
            qrycrt = qrycrt->nextsis;
            continue;
        }
        return YES;
    }
}

static bool qval2qry (FLD * qfld, FLD * qryfld) {
    FLD    * pftmp;
    FLD    * specfld = qryfld;
    CRT    * papa;
    byte   * pq, * pspecbuf, * pspec, * p, * q;
    byte     rangebuf [] = { '(', 10, 1, '&', 6 };
    IDFLD  * pidf;
    int      len = 0, len1, numopen = 0;
    bool     retcode = YES, first = YES, range = NO, figur = NO, sq = NO;
    bool     wasf = NO, empty = NO;
    long     ltmpnum = 0L;
    double   dtmpnum = 0.0;
    byte     b = 1;
/**************************************************************************
     32  16  8   4   2   1                    8   4   2
   ⁄ƒƒƒ¬ƒƒƒ¬ƒƒƒ¬ƒƒƒ¬ƒƒƒ¬ƒƒƒø                ⁄ƒƒƒ¬ƒƒƒ¬ƒƒƒø
   ≥   ≥   ≥   ≥   ≥   ≥   ≥                ≥   ≥   ≥   ≥
   ¿ƒ¬ƒ¡ƒ¬ƒ¡ƒ¬ƒ¡ƒ¬ƒ¡ƒ¬ƒ¡ƒ¬ƒŸ                ¿ƒ¬ƒ¡ƒ¬ƒ¡ƒ¬ƒŸ
     ≥   ≥   ≥   ≥   ≥   ¿ƒƒƒƒƒ  ë¢Ôß™†       ≥   ≥   ¿ƒƒƒƒƒ  =
     ≥   ≥   ≥   ≥   ¿ƒƒƒƒƒƒƒƒƒ  > <          ≥   ¿ƒƒƒƒƒƒƒƒƒ  <
     ≥   ≥   ≥   ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒ  !            ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒ  >
     ≥   ≥   ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ  ( { [
     ≥   ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ  ) } ]
     ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ  á≠†Á•≠®•
**************************************************************************/
    qryfld = specfld->next;
    qfld = qfld->next;
    pidf = dbgetidf (qdoc, qfld);
    papa = dbgetpapa (qfld);
    pftmp = qfld;

    do  len += pftmp->val.s.len;
    while ((pftmp = nextcont (pftmp, papa)) != NULL);

    if (len == 0) return YES;

    pspecbuf = calloc (len * 3 + 3, 1);   /* +3 - ÆÁ•≠Ï ¢†¶≠Æ!!! */
    pspec = pspecbuf + 1;
    pq = qfld->val.s.p;

    while (1) {
        switch (* pq) {
            case '[':
            case '{':
                if (sq || figur || numopen > 0 || (b & 2))
                    { retcode = NO; goto ret; }
                if (!(b & 5)) * pspec++ = '&';
                if (* pq == '{') figur = YES;
                else             sq    = YES;
                wasf = YES;
                b = 8;
                break;
            case ']':
                if (!sq || numopen > 0 || !(b & 48))
                    { retcode = NO; goto ret; }
                sq = NO;
                b = 16;
                break;
            case '}':
                if (!figur || numopen > 0 || !(b & 48))
                    { retcode = NO; goto ret; }
                figur = NO;
                b = 16;
                break;
            case '(':
                if ((b & 2)) { retcode = NO; goto ret; }
                if (!(b & 13)) * pspec++ = '&';
                b = 8;
                numopen++;
                break;
            case ')':
                if (!(b & 48)) { retcode = NO; goto ret; }
                b = 16;
                numopen--;
                break;
            case '!':
                if ((b & 6)) { retcode = NO; goto ret; }
                if (!(b & 9)) * pspec++ = '&';
                b = 4;
                break;
            case '>':
            case '<':
                if ((b & 6)) { retcode = NO; goto ret; }
                if (!(b & 9)) * pspec++ = '&';
                b = 2;
                * pspec++ = (* pq == '>') ? 8 : 4;
                pq++;
                continue;
            case '&':
            case '|':
                if ((b & 1)) { retcode = NO; goto ret; }
                b = 1;
                break;
            case ':':
                if (!(b & 32) || range || empty) { retcode = NO; goto ret; }
                if (pspec - 3 >= pspecbuf && (* (pspec - 2) | 14) == 14)
                    { retcode = NO; goto ret; }
                utmovabs (pspec - 1, rangebuf, 5, 0);
                pspec += 4; pq++;
                range = YES;
                b = 2;
                continue;
            case ' ':
                pq++;
                continue;
            case '=':
                if (b != 2 || (* (pspec - 1) & 2)) { retcode = NO; goto ret; }
                * (pspec - 1) |= 2;
                pq++;
                continue;
            case EOS:
                if ((pftmp = nextcont (qfld, papa)) == NULL) goto ret;
                qfld = pftmp;
                pq = qfld->val.s.p;
                continue;
            default :
                if ((b & 48)) * pspec++ = '&';
                if (* pq == '@') {
                    if ((b & 2)) { retcode = NO; goto ret; }
                    * pspec++ = * pq++;
                    empty = YES; b = 32;
                    continue;
                }
                * pspec++ = 1;
                if (!first)
                    qryfld = dbcrtmul (qrydoc, qryfld->curcrt, YES, NO);
                else first = NO;
                switch (pidf->valtype) {
                    case SL_CDF:
                    case SL_TEXT:
                        if (* pq++ != '\\') { retcode = NO; goto ret; }
                        len = 0; pftmp = qfld; q = pq;
                        while (1) {
                            if (((char *)p = strchr ((char *) q, '\\')) != NULL)
                                { len += (int) (p - q); break; }
                            len += strlen ((char *) q) + 1;
                            if ((pftmp = nextcont (pftmp, papa)) == NULL)
                                { retcode = NO; goto ret; }
                            q = pftmp->val.s.p;
                        }
                        utfree (&qryfld->val.s.p);
                        q = pq;
                        pq = p + 1;
                        p = qryfld->val.s.p = calloc (len + 1, 1);
                        while (1) {
                            len1 = strlen ((char *) q);
                            if (len1 > len) len1 = len;
                            utexblnk (p, q, len1);
                            len -= len1;
                            p += len1;
                            if (len == 0) {
                                qryfld->val.s.len = (int) (p - qryfld->val.s.p);
                                break;
                            }
                            * p++ = ' '; len--;
                            qfld = nextcont (qfld, papa);
                            q = qfld->val.s.p;
                        }
                        if (pidf->valtype == SL_TEXT) {
                            if (* qryfld->val.s.p == '*' && (b & 2))
                                { retcode = NO; goto ret; }
                        } else {
                            len1 = dbcdf2cod (pidf->fldlink.link.pdoc,
                                                              qryfld->val.s.p);
                            utfree (&qryfld->val.s.p);
                            qryfld->val.s.len = 0;
                            if (len1 == -1) { retcode = NO; goto ret; }
                            qryfld->val.i = len1;
                        }
                        break;
                    case SL_FLOAT:
                        if (!utc2dbl (&(char *) pq, &dtmpnum, NO))
                            { retcode = NO; goto ret; }
                        qryfld->val.f = (float) dtmpnum;
                        break;
                    case SL_DOUBLE:
                        if (!utc2dbl (&(char *) pq, &dtmpnum, YES))
                            { retcode = NO; goto ret; }
                        qryfld->val.d = dtmpnum;
                        break;
                    case SL_MONEY:
                        break;
                    case SL_DATE:
                        if ((ltmpnum = utc2date (&(char *) pq,
                                           pidf->valopt.date.centure)) == -1L)
                            { retcode = NO; goto ret; }
                        qryfld->val.l = ltmpnum;
                        break;
                    case SL_LONG:
                        if (!utc2lrdx (&(char *) pq, &ltmpnum, 10))
                            { retcode = NO; goto ret; }
                        qryfld->val.l = ltmpnum;
                        break;
                    case SL_INT:
                        if (!utc2lrdx (&(char *) pq, &ltmpnum, 10))
                            { retcode = NO; goto ret; }
                        qryfld->val.i = (int) ltmpnum;
                        break;
                    default :
                        if (Db_Fn_q2qry == NULL) continue;
                        if (!(* Db_Fn_q2qry) (qryfld, &(char *) pq,
                                                                pidf->valtype))
                            { retcode = NO; goto ret; }
                        break;
                }
                if (range) { * pspec++ = ')'; b = 16; }
                else b = 32;
                continue;
        }
        range = empty = NO;
        * pspec++ = * pq++;
    }
ret:
    if (numopen != 0 || sq || figur) retcode = NO;
    if (retcode) {
        if (wasf) psos = pspecbuf + 1;
        else {
            * pspecbuf = '[';
            * pspec    = ']';
            psos = pspecbuf;
        }
        pfndfld = specfld;
        specfld->val.q = makefrg ();
        qryfld->prnopt.full = specfld->prnopt.full = YES;
    } else qdoc->curfld = qfld;
    free (pspecbuf);
    return retcode;
}

static FRG * makefrg (void) {
    FRG     * pfrg, * pfrgprev = NULL, * pfrgfirst;
    FND     * pfnd, * pfndprev = NULL;
    int       code, fig;
    bool      neg;

beg:
    pfrg = utalloc (FRG);
    if (pfrgprev == NULL) pfrgfirst = pfrg;
    else             pfrgprev->next = pfrg;
    code = 2; fig = 0;
    neg = NO;

    while (1) {
        switch (* psos) {
            case '|':
                psos++;
                pfrgprev = pfrg;
                goto beg;
            case '!':
                neg = YES;
                break;
            case  2 :
            case  4 :
            case  6 :
            case  8 :
            case 10 :
                code = * psos;
                break;
            case '@':
            case  1 :
                pfrg->nfrg++;
                pfrg->ngrid++;
                pfnd = utalloc (FND);
                if (pfrg->first == NULL)  pfrg->first = pfnd;
                else pfndprev->next = pfnd;
                pfndprev = pfnd;
                if (* psos == 1) {
                    pfndfld = pfndfld->next;
                    pfnd->pfld = pfndfld;
                }
                pfnd->neg  = neg;
                pfnd->code = code;
                neg = NO; code = 2;
                break;
            case '{':
                fig = 1;
            case '[':
            case '(':
                psos++;
                pfrg->nfrg++;
                if (fig == 0) pfrg->ngrid++;
                pfnd = utalloc (FND);
                if (pfrg->first == NULL)  pfrg->first = pfnd;
                else pfndprev->next = pfnd;
                pfndprev = pfnd;
                pfnd->neg  = neg;
                pfnd->code = fig;
                neg = NO; code = 2; fig = 0;
                pfnd->pfrg = makefrg ();
                break;
            case '}':
            case ')':
            case ']':
            case EOS:
                return pfrgfirst;
            default :     break;
        }
        psos++;
    }
}

void dbclrfrg (DOC * doc) {
    FLD * pcurfld  = doc->crtroot->fld_first;
    FLD * plastfld = doc->crtroot->fld_last;

    while (1) {
        if (pcurfld->idn == 0 && pcurfld->prnopt.full) {
            clrfrg ((FRG *) pcurfld->val.q);
            pcurfld->val.q = NULL;
        }
        if (pcurfld == plastfld) return;
        pcurfld = pcurfld->next;
    }
}

static void clrfrg (FRG * pfrg) {
    FND     * pfndnext, * pfnd;
    FRG     * pfrgnext;

    while (1) {
        pfnd = pfrg->first;
        pfrgnext = pfrg->next;
        while (1) {
            pfndnext = pfnd->next;
            if (pfnd->pfrg != NULL) clrfrg (pfnd->pfrg);
            free (pfnd);
            if (pfndnext == NULL) break;
            pfnd = pfndnext;
        }
        free (pfrg);
        if (pfrgnext == NULL) return;
        pfrg = pfrgnext;
    }
}

