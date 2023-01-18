#include <sldbas.h>
#include <alloc.h>

static bool     loadcrt      (CRT * pcrt);
static void     pwrktoval    (FLD * pfld);
static void     file2pwrk    (void);
static void     dbflush      (void);
static void     valtopwrk     (FLD * pfld);
static void     savecrt      (CRT * pcrt);
static long     gettotalboff (IDBASE * pdb);
static long     getoffboff   (long numrec);
static void     frompwrk     (void * to, int len);
static void     topwrk       (void * from, int len);
static bool     _savrec      (IDBASE * pidb, CRT * pcrt, bool need_close);


static  DOC     * doc;
static  byte    * p;
static  int       taillen;
static  long      reclen;
static  int       handb;
static  PRNOPT    prnopt;
/** if (prnopt.spec) {              ******************************/
/**     if (prnopt.full)          - конец записи *****************/
/**     else if (prnopt.jst == 1) - конец автопереносной колонки */
/**     else                      - команда на множение **********/
/** }                               ******************************/

bool dblodone (DOC * pdoc, long numrec, bool forward) {
    IDBASE  * pdb;
    bool      retcode;

    pdb = dbopendb (pdoc->name_data, pdoc->opt._global, UT_R);
    retcode = dblodrec (pdoc, pdb, numrec, forward);
    dbclosdb (pdb);
    return retcode;
}

bool dblodrec (DOC * pdoc, IDBASE * pidb, long numrec, bool forward) {
    long    boffset;

    doc  = pdoc;
    handb = pidb->handb;
    WORK;

    if ((boffset = findrec (pidb, &numrec, forward)) == -1L) return NO;
    utlseek (handb, boffset, UT_BEG);

    if (!dbclrdoc (doc, NO)) return NO;
    utread (handb, &reclen, sizeof (long));
    reclen -= sizeof (long);
    taillen = 0;
    file2pwrk ();
    if (!loadcrt (doc->crtroot)) { dbclrdoc (doc,NO); return NO; }
    doc->numrec = numrec;
    return YES;
}

bool dblodcrt (DOC * pdoc, CRT * pcrt, IDBASE * pidb, long numrec) {
    long    boffset;

    doc  = pdoc;
    handb = pidb->handb;
    WORK;

    if ((boffset = findrec (pidb, &numrec, YES)) == -1L) return NO;
    utlseek (handb, boffset, UT_BEG);

    utread (handb, &reclen, sizeof (long));
    reclen -= sizeof (long);
    taillen = 0;
    file2pwrk ();
    if (!loadcrt (pcrt)) return NO;
    doc->numrec = numrec;
    return YES;
}

bool dbloddoc (DOC * pdoc, int hand) {

    doc  = pdoc;
    handb = hand;
    WORK;

    dbclrdoc (doc, NO);
    reclen = 500000L;
    taillen = 0;
    file2pwrk ();
    return (loadcrt (doc->crtroot));
}

static bool loadcrt (CRT * pcrt) {
    FLD     * curfld, * lastfld;
    int       idc;

    while (1) {
        curfld  = pcrt->fld_first;
        lastfld = pcrt->fld_last;

        while (1) {
            if (!curfld->opt.done && curfld->curcrt == pcrt) pwrktoval (curfld);
            if (curfld == lastfld) break;
            curfld = curfld->next;
        }
        if (pcrt->lowcrt  != NULL)
            if (!loadcrt (pcrt->lowcrt)) return NO;
        if (taillen < 3) file2pwrk ();
        if (((PRNOPT *) p)->spec) {
            if (((PRNOPT *) p)->full) return YES;
            idc = (int) (* (p + 2));
            if (pcrt->idc != idc) return YES;
            p += 3;
            taillen -= 3;
            if ((curfld = dbcrtmul (doc, pcrt, YES, NO)) == NULL) return NO;
            while (curfld->curcrt->idc != idc) curfld = curfld->next;
            pcrt = curfld->curcrt;
            continue;
        } else if (pcrt->nextsis != NULL) {
            pcrt = pcrt->nextsis;    continue;
        }
        return YES;
    }
}

static void pwrktoval (FLD * pfld) {
    IDCOL       * curcol;
    TXTVAL      * ptxtval;
    IDFLD       * pidf;
    void        * v;
    byte        * q;
    PRNOPT        tmpopt;
    int           vallen = 0, i;

    pidf = dbgetidf (doc, pfld);
    if (pidf->valtype == SL_TEXT) {
        if (pfld->opt.wrap) {
            dboutcol (doc, pfld, NO);
            for (curcol = firstcol; curcol != NULL; curcol = curcol->next) {
                ptxtval = curcol->pval;
                frompwrk (&tmpopt, 2);
                while (1) {
                    ptxtval->prnopt = tmpopt;
                    if (ptxtval->prnopt.full) {
                        frompwrk (&vallen, 2);
                        ptxtval->val.s.len = vallen;
                        q = ptxtval->val.s.p = calloc (vallen + 1, 1);
                        while (1) {
                            if (taillen < vallen) {
                                i = taillen;
                                frompwrk (q, taillen);
                                vallen -= i;
                                q += i;
                                file2pwrk ();
                            } else {
                                frompwrk (q, vallen);
                                q [vallen] = EOS;
                                break;
                            }
                        }
                    }
                    frompwrk (&tmpopt, 2);
                    if (tmpopt.spec && tmpopt.jst == 1) break;
                    ptxtval = valbreak (ptxtval, NULL, 0);
                    ptxtval->par = YES;
                }
            }
            dbfmtcol (NO);
            return;
        }
        utfree (&pfld->val.s.p);
        frompwrk (&pfld->prnopt, 2);
        if (pfld->prnopt.full) {
            frompwrk (&vallen, 2);
            pfld->val.s.len = vallen;
            q = pfld->val.s.p = calloc (vallen + 1, 1);
            while (1) {
                if (taillen < vallen) {
                    frompwrk (q, taillen);
                    vallen -= taillen;
                    file2pwrk ();
                } else {
                    frompwrk (q, vallen);
                    q [vallen] = EOS;
                    break;
                }
            }
        } else  pfld->val.s.len = 0;
    } else {
        switch (pidf->valtype) {
            case SL_FLOAT:
                vallen = sizeof (float);
                (float *) v = &pfld->val.f;
                break;
            case SL_DATE:
                vallen = sizeof (long);
                (long *) v = &pfld->val.l;
                break;
            case SL_LONG:
                vallen = sizeof (long);
                (long *) v = &pfld->val.l;
                break;
            case SL_CDF:
            case SL_INT:
                vallen = sizeof (int);
                (int *) v = &pfld->val.i;
                break;
            case SL_MONEY:
            case SL_DOUBLE:
            default :
                vallen = sizeof (double);
                (double *) v = &pfld->val.d;
                break;
        }
        frompwrk (&pfld->prnopt, 2);
        if (pfld->prnopt.full) frompwrk (v, vallen);
    }
    return;
}

static void frompwrk (void * to, int len) {
    if (taillen < len) file2pwrk ();
    utmovabs (to, p, len, 0); p += len; taillen -= len;
    return;
}

static void file2pwrk (void) {

    if (reclen == 0L) return;
    utlseek (handb, (long) (-taillen), UT_CUR);
    p = Db_pwrk;
    if (reclen < (long) PWRK_LEN) taillen = (int) reclen;
    else                          taillen = PWRK_LEN;
    utread (handb, p, taillen);
    reclen -= taillen;
    return;
}

bool dbsavrec (DOC * pdoc, CRT * pcrt) {
    IDBASE  * pidb;
    bool      retcode;

    doc = pdoc;

    pidb = dbopendb (pdoc->name_data, pdoc->opt._global, UT_WR);

    retcode = _savrec (pidb, pcrt, YES);

    free (pidb);
    return retcode;
}

bool dbsavtmp (DOC * pdoc, CRT * pcrt, IDBASE * pidb) {

    doc = pdoc;

    return (_savrec (pidb, pcrt, NO));
}

static bool _savrec (IDBASE * pidb, CRT * pcrt, bool need_close) {
    long      begrec = 0L, numrec, boffset, len, num;

    num = numrec = doc->numrec;
    handb = pidb->handb;
    WORK;

    if (numrec > 0 && (boffset = findrec (pidb, &num, YES)) == -1L)
        return NO;

    if (numrec == 0L) {
        pidb->slhd.total++;
        pidb->lenh += sizeof (long);
        numrec = gettotalboff (pidb);
    } else if (num != numrec)
        pidb->slhd.total++;
    else {
        utlseek (handb, boffset, UT_BEG);
        utread  (handb, &len, sizeof (long));
        pidb->slhd.len -= len;
    }
    begrec = utlseek (handb, 0L, UT_END);

    dbclrdone (pcrt);
    p = Db_pwrk;
    prnopt.spec = YES;
    prnopt.full = NO;
    taillen = PWRK_LEN;
    topwrk (&len, sizeof (long));

    savecrt (pcrt);
    dbflush ();
    prnopt.full = YES;
    utwrite (handb, &prnopt, 2);

    len = utlseek (handb, 0L, UT_CUR) - begrec;
    pidb->slhd.len += len;
    utlseek (pidb->handb, begrec, UT_BEG);
    utwrite (pidb->handb, &len, sizeof (long));

    if (need_close) utclose (pidb->handb);
                                    /* Прописали запись в базу и закрыли ее */

    utlseek (pidb->handh, getoffboff (numrec), UT_BEG);
    utwrite (pidb->handh, &begrec, sizeof (long));
    pidb->slhd.date = utdtoday ();
    utlseek (pidb->handh, 0L, UT_BEG);
    utwrite (pidb->handh, &pidb->slhd, sizeof (SLHD));
    doc->numrec = numrec;
                              /***?????? begrec может запортиться при выкл. **/
    if (need_close) utclose (pidb->handh);
    else if (numrec >= pidb->firstboff && numrec <= pidb->firstboff+NUM_OFF-1)
        pidb->pboff [(int) (numrec - pidb->firstboff)] = begrec;

    return YES;
}

void dbsavdoc (DOC * pdoc, CRT * pcrt, int hand) {

    doc = pdoc;
    handb = hand;
    WORK;

    dbclrdone (pcrt);
    p = Db_pwrk;
    prnopt.spec = YES;
    prnopt.full = NO;
    taillen = PWRK_LEN;
                                /* В начале записи не пишем !!! длину записи */
    savecrt (pcrt);
    dbflush ();
    prnopt.full = YES;

    utwrite (handb, &prnopt, 2);

    return;
}

static void savecrt (CRT * pcrt) {
    FLD     * curfld, * lastfld;

    while (1) {
        curfld  = pcrt->fld_first;
        lastfld = pcrt->fld_last;

        while (1) {
            if (!curfld->opt.done && curfld->curcrt == pcrt) valtopwrk (curfld);
            if (curfld == lastfld) break;
            curfld = curfld->next;
        }
        if (pcrt->lowcrt  != NULL) savecrt (pcrt->lowcrt);
        if (pcrt->next != NULL &&
                           (!pcrt->opt.multy || !pcrt->fld_first->opt.wrap)) {
            topwrk (&prnopt, 2);
            topwrk (&pcrt->idc, 1);
            pcrt = pcrt->next;       continue;
        } else if (pcrt->nextsis != NULL) {
            pcrt = pcrt->nextsis;    continue;
        }
        return;
    }
}

static void valtopwrk (FLD * pfld) {
    IDCOL       * curcol;
    TXTVAL      * ptxtval;
    IDFLD       * pidf;
    void        * v = NULL;
    int           vallen = 0;

    pidf = dbgetidf (doc, pfld);
    switch (pidf->valtype) {
        case SL_TEXT:
            if (pfld->opt.wrap) {
                dboutcol (doc, pfld, YES);
                prnopt.jst = 1;
                for (curcol = firstcol; curcol != NULL; curcol = curcol->next){
                    ptxtval = curcol->pval;
                    do {
                        vallen = ptxtval->val.s.len;
                        topwrk (&ptxtval->prnopt, 2);
                        if (vallen > 0) {
                            topwrk (&vallen, 2);
                            if (taillen < vallen) {
                                dbflush ();
                                utwrite (handb, ptxtval->val.s.p, vallen);
                            } else topwrk (ptxtval->val.s.p, vallen);
                        }
                        ptxtval = ptxtval->next;
                    } while (ptxtval != curcol->pval);
                    topwrk (&prnopt, 2);
                }
                prnopt.jst = 0;
                colfree ();
                break;
            }
            vallen = pfld->val.s.len;
            topwrk (&pfld->prnopt, 2);
            if (pfld->prnopt.full) {
                topwrk (&vallen, 2);
                if (vallen > taillen) {
                    dbflush ();
                    utwrite (handb, (char *) pfld->val.s.p, vallen);
                } else topwrk (pfld->val.s.p, vallen);
            }
            break;
        default :
            switch (pidf->valtype) {
                case SL_FLOAT:
                    vallen = sizeof (float);
                    (float *) v = &pfld->val.f;
                    break;
                case SL_DATE:
                    vallen = sizeof (long);
                    (long *) v = &pfld->val.l;
                    break;
                case SL_LONG:
                    vallen = sizeof (long);
                    (long *) v = &pfld->val.l;
                    break;
                case SL_CDF:
                case SL_INT:
                    vallen = sizeof (int);
                    (int *) v = &pfld->val.i;
                    break;
                case SL_MONEY:
                case SL_DOUBLE:
                default :
                    vallen = sizeof (double);
                    (double *) v = &pfld->val.d;
                    break;
            }
            topwrk (&pfld->prnopt, 2);
            if (pfld->prnopt.full) topwrk (v, vallen);
    }
    return;
}

static void dbflush (void) {

    utwrite (handb, Db_pwrk, (int) (p - Db_pwrk));
    if (mnharder (NULL, 0) != -1) { dbfindb (); exit (92); }
    p = Db_pwrk;
    taillen = PWRK_LEN;
}

static void topwrk (void * from, int len) {

    if (taillen < len) dbflush ();
    utmovabs (p, from, len, 0);   p += len;   taillen -= len;
}

bool   dbdelone (DOC * doc) {
    IDBASE  * pdb;
    bool      retcode;

    pdb = dbopendb (doc->name_data, doc->opt._global, UT_WR);
    retcode = dbdelrec (pdb, doc->numrec);
    dbclosdb (pdb);
    return retcode;
}

bool    dbdelrec (IDBASE * pdb, long numrec) {
    long      boffset, num = numrec;

    WORK;
    if ((boffset = findrec (pdb, &num, YES)) == -1L) return NO;
    if (num != numrec) return NO;

    pdb->slhd.total--;
    pdb->slhd.date = utdtoday ();
    utlseek (pdb->handb, boffset, UT_BEG);
    utread (pdb->handb, &reclen, sizeof (long));
    pdb->slhd.len -= reclen;
    if (pdb->slhd.total > 0L) {
        boffset = -1L;
        utlseek (pdb->handh, getoffboff (numrec), UT_BEG);
        utwrite (pdb->handh, &boffset, sizeof (long));
    } else {
        utchsize (pdb->handh, 0L);
        utchsize (pdb->handb, (long) pdb->slhd.treelen);
    }
    utlseek (pdb->handh, 0L, UT_BEG);
    utwrite (pdb->handh, &pdb->slhd, sizeof (SLHD));

    return YES;
}

IDBASE  *  dbopendb (char * name_data, bool db_global, int mode) {
    IDBASE  * pdb;

    if (* name_data == EOS) return NULL;
    if ((pdb = utalloc (IDBASE)) == NULL) return NULL;
    pdb->handh = -1;

    if ((pdb->handb = dbopen (name_data, DB_BAS,  db_global, mode)) == -1)
        goto ret;
    if (mode == UT_W) mode = UT_RW;
    if ((pdb->handh = dbopen (name_data, DB_HEAD, db_global, mode)) == -1)
        goto ret;

    utread (pdb->handh, &pdb->slhd, sizeof (SLHD) + sizeof (long) * NUM_OFF);
    pdb->lenh = utlseek (pdb->handh, 0L, UT_END);
    pdb->firstboff = 1L;

    return pdb;
ret:
    utclose (pdb->handb);
    utclose (pdb->handh);
    free (pdb);
    return NULL;
}

void dbclosdb (IDBASE * pdb) {

    if (pdb == NULL) return;
    utclose (pdb->handb);   utclose (pdb->handh);
    free (pdb);
    return;
}

void dbcreatmp (char * basenam) {
    SLHD        * pslhd;
    char          path [80];
    register      hand;

    pslhd = utalloc (SLHD);

    strcpy (path, basenam);
    utchext (path, Db_ext [DB_BAS]);

    hand = utcreate (path);
    utclose (hand);

    utchext (path, Db_ext [DB_HEAD]);
    hand = utcreate (path);
    utwrite (hand, pslhd, sizeof (SLHD));
    utclose (hand);

    free (pslhd);
}

static long gettotalboff (IDBASE * pdb) {
    return (pdb->lenh - sizeof (SLHD)) / sizeof (long);
}

static long getoffboff (long numrec) {
    return (sizeof (SLHD) + sizeof (long) * (numrec - 1));
}

long  findrec (IDBASE * pdb, long * pnumrec, bool forward) {
    long         numrec = * pnumrec, firstnumrec, boffset;
    long         lastboff, delta = NUM_OFF - 1;

    lastboff = gettotalboff (pdb);

    while (1) {
        if (numrec > lastboff || numrec < 1) return -1L;
        if (numrec >= pdb->firstboff && numrec <= pdb->firstboff+NUM_OFF-1) {
            boffset =  pdb->pboff [(int) (numrec - pdb->firstboff)];
            if (boffset >= 0L) {
                * pnumrec = numrec;
                return boffset;
            } else if (forward) numrec++;
            else numrec--;
            continue;
        } else if (forward) {
            firstnumrec = numrec;
            if (numrec + delta > lastboff) firstnumrec = lastboff - delta;
        } else firstnumrec = numrec - delta;
        if (firstnumrec < 1) firstnumrec = 1;
        utlseek (pdb->handh, sizeof (SLHD) + (firstnumrec - 1) * sizeof (long),
                                                                        UT_BEG);
        utread (pdb->handh, (char *) pdb->pboff, NUM_OFF * sizeof (long));
        pdb->firstboff = firstnumrec;
    }
}

