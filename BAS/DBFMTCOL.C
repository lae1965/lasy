#include <sldbas.h>

IDCOL           * firstcol;
TXTVAL          * curval;

static DOC      * doc;
static IDCOL    * curcol;
static CRT      * papa;
static TXTVAL   * lastval;
static byte     * q;
static int        wordlen;


int       getword   (bool first);
void      colfree   (void);
void      dbval2col (bool first);
void      val2fld   (FLD * pfld, TXTVAL * pv, byte * pb, int len, int fldlen);
int       dbfitval  (void);
void      uvval2col (bool first);
void      uvfitval  (void);
FLD     * multcols  (FLD * pfcur);

static IDCOL    * outval    (FLD * pfld);
static IDCOL    * val2save  (FLD * pfld);
static int        getfldlen (FLD * pfld, PRNOPT * prnopt);

void dbprpval (int fldlen) {
    FLD     * pftmp;
    byte    * p, * pbeg = curval->val.s.p, * pfrom;
    int       upremain;
    register  curvallen = curval->val.s.len, i = 0;
    bool      blank = NO;

    if (doc->pos < curvallen) {
         if (pbeg [doc->pos] != ' ' || doc->pos == 0 ||
                                         pbeg [doc->pos - 1] == ' ') return;
         blank = YES; /* стоим на разделительном пробеле */
    }
    pfrom = pbeg + curvallen;
    if (doc->pos >= fldlen) {
        if (doc->pos > curvallen) goto brk;
        p = pbeg + curvallen - 1;
        while (* p != ' ') {
            if (p == pbeg) goto brk;
            p--;
        }
        while (* p == ' ') {
            if (p == pbeg) goto brk;
            p--;
        }
        pfrom = p + 2;
        i = curvallen - (int) (pfrom - pbeg);
        goto brk;
    } else  if (!curval->par) {
        pftmp = prevcont (doc->curfld, papa);
        dbbeglen (doc, pftmp, NULL, &upremain);
        upremain -= curval->prev->val.s.len;
        if (upremain >= doc->pos + 2) return;
                        /* маркер влазит в верхнее поле */
        if (blank && upremain == doc->pos + 1) { curval->newfld = YES; return; }
                        /* стоим на разделит. пробеле, маркер не влазит */
        if (upremain < curvallen + 1) return;
                        /* содержимое поля не влазит даже без маркера */
        if (doc->pos - curvallen > 0) {
brk:
            valbreak (curval, pfrom, i);
        }
        curval->newfld = YES;
    }
    return;
}

TXTVAL  * valbreak (TXTVAL * oldval, byte * p, int len) {
    TXTVAL * pval;

    pval = utalloc (TXTVAL);
    pval->prev = oldval;
    pval->next = oldval->next;
    oldval->next->prev = pval;
    oldval->next = pval;
    pval->prnopt = oldval->prnopt;
    if (oldval == curval) {
        if (doc->pos >= (int) (p - oldval->val.s.p)) {
            curval = pval;
            doc->pos -= (int) (p - oldval->val.s.p);
        }
    }
    if (len == 0) { doc->pos = 0; goto ret; }
    pval->val.s.len = len;
    pval->val.s.p = malloc (len);
    utmovabs (pval->val.s.p, p, len, 0);
    while (p != oldval->val.s.p) {
        p--;
        if (* p != ' ') break;
        len++;
    }
    oldval->val.s.len -= len;
ret:
    if (oldval->val.s.len == 0) oldval->par = pval->par = YES;
    return pval;
}

void dboutcol (DOC * pdoc, FLD * pfld, bool for_save) {
    CRT     * pcrt;
    IDCOL   * pidcol;

    doc = pdoc;
    papa = dbgetpapa (pfld);

    if (for_save) pidcol = val2save (pfld);
    else          pidcol = outval   (pfld);

    firstcol = pidcol;

    /* Если в этой колонке существует mult, выдираем колонки всех его полей */
    if ((pfld = firstcol->mult) != NULL) {
        pcrt = pfld->curcrt;
        while (pfld != pcrt->fld_first) {
            pfld = pfld->prev;
            if (for_save) pidcol->next = val2save (pfld);
            else          pidcol->next = outval   (pfld);
            pidcol = pidcol->next;
        }
        pfld = firstcol->mult;
        while (pfld != pcrt->fld_last) {
            pfld = pfld->next;
            if (for_save) pidcol->next = val2save (pfld);
            else          pidcol->next = outval   (pfld);
            pidcol = pidcol->next;
        }
    }
    doc->opt.change = YES;
    return;
}

static IDCOL * outval (FLD * pfld) {
    IDCOL   * pidcol;
    TXTVAL  * ptxtval, * prevval = NULL;
    FLD     * pftmp;

    pidcol = utalloc (IDCOL);

    while (1) {
        pftmp = prevcont (pfld, papa);
        if (pftmp == NULL) break;
        pfld = pftmp;
    }

    pfld->opt.par = YES;
    pidcol->first = pfld;

    while (1) {
        if (doc->curfld != pfld && !pfld->prnopt.full) {
            if (!pfld->opt.par) goto nextfld;
            if ((pftmp = nextcont (pfld, papa)) != NULL && !pftmp->opt.par) {
                pftmp->opt.par = YES; goto nextfld;
            }
        }
        ptxtval = utalloc (TXTVAL);
        ptxtval->val = pfld->val;
        pfld->val.s.p = NULL;
        pfld->val.s.len = 0;
        if (pfld == doc->curfld) curval = ptxtval;
        ptxtval->par = pfld->opt.par;
        ptxtval->prnopt = pfld->prnopt;
        if (prevval == NULL) pidcol->pval = ptxtval;
        else {
            prevval->next = ptxtval;
            ptxtval->prev = prevval;
        }
        prevval = ptxtval;
nextfld:
        if (pidcol->mult == NULL && pfld->curcrt->opt.multy)
            pidcol->mult = pfld;
        pftmp = nextcont (pfld, papa);
        if (pftmp == NULL) break;
        pfld = pftmp;
    }

    pidcol->last = pfld;
    ptxtval->next = pidcol->pval;
    pidcol->pval->prev = ptxtval;

    return pidcol;
}

int dbfmtcol (bool need) {
    CRT       * pcrt;
    FLD       * pftmp;
    register    i = 32000, j = 0;
    int         numline = 0;
    bool        visi = doc->opt.visibl;

    if (need) numline = dbcalcln (doc, firstcol->first, doc->curfld);

    /* Размещаем текст из валов в каждую колонку от 1-го поля до mult'а */
    for (curcol = firstcol; curcol != NULL; curcol = curcol->next) {
        if (visi) dbval2col (YES);
        else      uvval2col (YES);
    }

    /* Примеряем каждую колонку */
    for (curcol = firstcol; curcol != NULL; curcol = curcol->next) {
        if (visi) j = dbfitval ();
        else          uvfitval ();
        if (j < i) i = j;
    }

    /* Удаляем лишние поля */
    for (; i > 0; i--) {
        pcrt = firstcol->mult->curcrt->next;
        if (pcrt->next == NULL) {
            for (curcol = firstcol; curcol != NULL; curcol = curcol->next) {
                pftmp = nextcont (curcol->mult, papa);
                if (pftmp == curcol->last) curcol->last = curcol->mult;
            }
        }
        dbcrtdel (doc, pcrt);
    }

    /* Размещаем текст из валов в каждую колонку от mult->next до последнего */
    for (curcol = firstcol; curcol != NULL; curcol = curcol->next) {
        if (visi) dbval2col (NO);
        else      uvval2col (NO);
    }

    if (need) numline = dbcalcln (doc, firstcol->first, doc->curfld) - numline;

    /* Освобождаем созданные структуры */
    colfree ();
    return numline;
}

void colfree (void) {
    TXTVAL     * pval, * nextval;
    IDCOL      * pidcol;

    do {
        pval = firstcol->pval;
        do {
            nextval = pval->next;
            free (pval->val.s.p);
            free (pval);
            pval = nextval;
        } while (pval != firstcol->pval);
        pidcol = firstcol->next;
        free (firstcol);
        firstcol = pidcol;
    } while (firstcol != NULL);

    curval = NULL;

    return;
}

void  dbval2col (bool first) {
    FLD     * pfcur;
    TXTVAL  * pval, * newval;
    byte    * pbeg, * ptmp, * pnewbeg;
    register  newlen, vallen;
    int       fldlen, fldremain;
    bool      fldempt;

    /* fldremain - длина поля, vallen - длина vala */
    /* newval  - val, с которого начинается заполнение поля */
    /* pnewbeg - указатель в newval, с которого начинается заполнение поля */
    /* newlen  - новая длина поля */

    if (first) {
        pfcur  = curcol->first;
        pval   = curcol->pval;
    } else {
        pfcur = curcol->mult;
        if (curcol->savval == NULL) {
            if (pfcur == NULL) return;
            goto ret;
        }
        pfcur  = nextcont (pfcur, papa);
        pval   = curcol->savval->next;
    }

    pbeg   = pval->val.s.p;
    vallen = pval->val.s.len;


    while (1) {        /* от первого поля до множественного, включая его */
        fldempt = YES;
        newval = pval;
        newlen = 0;
        pnewbeg = pbeg;

        fldlen = getfldlen (pfcur, &pval->prnopt); /* длина текущего поля  */
        fldremain = fldlen;

        while (1) {
            if (fldremain < vallen) {                 /* поле короче vala     */
                ptmp = pbeg + fldremain;             /* встаем на длину поля */

                while (* ptmp != ' ') {  /* возвращаемся на начало слова */

                    if (ptmp == pbeg) {   /* слово оказалось длиннее поля */
                        if (fldempt) {
                            newlen = fldremain;
                            vallen -= fldremain;
                            pbeg += fldremain;
                        } else newlen--; /* убираем зарезервированный пробел */
                        goto nextfld;
                    }
                    ptmp--;
                }

                while (* ptmp == ' ') {    /* на конец предыдущего слова   */

                    if (ptmp == pbeg) {       /* слово оказалось длиннее поля */
                        if (fldempt) {
                            newlen = fldremain;
                            vallen -= fldremain;
                            pbeg += fldremain;
                        } else newlen--; /* убираем зарезервированный пробел */
                        goto nextfld;
                    }
                    ptmp--;
                }
                vallen -= (int) (ptmp - pbeg) + 2;
                newlen += (int) (ptmp - pbeg) + 1;
                pbeg = ptmp + 2;
                break;
            } else {                             /* поле длиннее vala */
           /*     if (!fldempt && pval == curval) curval = newval; */
                fldremain -= vallen;
                newlen += vallen;
                fldempt = NO;
nextval:
                if (vallen == 0 && pval == curval) {
                    curval = newval;
                    doc->pos = newlen;
                }
                curcol->savval = pval;
                pval = pval->next;                 /* берем следующий val */
                if (pval == curcol->pval) {        /* все val'ы обработаны */
                    val2fld (pfcur, newval, pnewbeg, newlen, fldlen);
                    curcol->savval = NULL;
                    goto ret;
                }
                pbeg = pval->val.s.p; vallen = pval->val.s.len;
                if (pval->par || pval->newfld) break;
                if (vallen == 0) goto nextval;
                if (fldremain == 0) break;
                            /* новый поле */
                if (newlen > 0) { newlen++; fldremain--; }
            }
        }

nextfld:
        val2fld (pfcur, newval, pnewbeg, newlen, fldlen);
        if (pfcur == curcol->mult) break;
        if (pfcur == curcol->last) { curcol->savval = NULL; return; }
        pfcur = nextcont (pfcur, papa);
    }
    if (pbeg != pval->val.s.p) {
        curcol->savval = pval;
        valbreak (pval, pbeg, vallen);
    }
    return;

ret:
    while ((pfcur = nextcont (pfcur, papa)) != NULL) {
        pfcur->prnopt.full = pfcur->opt.par = NO;
        pfcur->opt.done = YES;
    }
    return;
}

void val2fld (FLD * pfld, TXTVAL * pval, byte * pbeg, int len, int fldlen) {
    byte      * p;
    register    i, j;
    int         remain = fldlen - len, vallen = len;
    bool        valdone = YES;

    pfld->opt.change = NO;
    pfld->opt.par = pval->par;
    if (pval->par) curcol->prnopt = pval->prnopt;
    pfld->prnopt = curcol->prnopt;
    if (len == 0) {
        pfld->prnopt.full = NO;
        pfld->opt.done = YES;
        if (pval == curval) { doc->curfld = pfld; pfld->opt.change = YES; }
        goto ret;
    }

    p = pfld->val.s.p = malloc (len + 1);

    while (1) {
        j = (int) (pbeg - pval->val.s.p);
        i = pval->val.s.len - j;
        if (i > len) { i = len; valdone = NO; }
        if (pval == curval && doc->pos >= j) {
            if (doc->pos >= pval->val.s.len && i == len && valdone &&
                                                                remain == 0) {
                if (doc->pos >= fldlen) doc->pos = fldlen - 1;
                curval = NULL;
                doc->curfld = pfld;
                pfld->opt.change = YES;
            }
            if (doc->pos < pval->val.s.len && pval->val.s.p [doc->pos] != ' ')
                remain = 0;
            if (doc->pos < j + len + remain) {
                doc->pos = vallen - len + doc->pos - j;
                doc->curfld = pfld;
                pfld->opt.change = YES;
                curval = NULL;
            }
        }
        if (i > 0) {
            utmovabs (p, pbeg, i, 0);
            p += i;
            len -= i;
            if (len == 0) break;
            * p++ = ' ';
            len--;
        }
        pval = pval->next;
        pbeg = pval->val.s.p;
    }
    pbeg = pfld->val.s.p + vallen - 1;
    while (* pbeg == ' ') {
        if (pbeg == pfld->val.s.p) {
            utfree (&pfld->val.s.p);
            pfld->opt.par = pval->par = YES;
            goto ret;
        }
        pbeg--;
    }

    vallen = (int) (pbeg - pfld->val.s.p) + 1;
    pfld->val.s.len = vallen;
    pfld->val.s.p [vallen] = EOS;
    pfld->prnopt.full = YES;
    if (pfld->prnopt.jst == UT_FORMAT) {
        if (valdone && (pval->next->par || pval->next == curcol->pval))
            pfld->val.s.word_1 = pfld->val.s.blank = 0;
        else {
            pfld->val.s.word_1 = dbclcwrd (pfld->val.s.p);
            pfld->val.s.blank = fldlen - vallen;
        }
    }
    pval->par = NO;
ret:
    pfld->opt.done = YES;
    return;
}

int dbfitval (void) {
    FLD     * pfcur = curcol->last;
    CRT     * pcmul, * pccur = NULL;
    register  i, oldfldlen = 0;
    int       fldlen;
    bool      fldempt, wasbreak = NO;


    if (curcol->mult == NULL) return 0;
    if (curcol->savval == NULL) goto end;

    wordlen = 0;
    lastval = curcol->pval->prev;
    q = lastval->val.s.p + lastval->val.s.len - 1;
    if (lastval->val.s.len == 0)  q++;

    i = getword (YES);
    if (i == 2) goto end;
    if (i == 1) goto prevfld;

    while (1) {     /* от последнего поля до множественного, не включая его */
        fldempt = YES;
        pfcur = multcols (pfcur);
        fldlen = getfldlen (pfcur, &lastval->prnopt); /* длина текущего поля  */

        while (1) {
            if (wordlen > fldlen) {
                if (fldempt) {
                    if (wordlen <= fldlen + oldfldlen) goto newword;
                    wordlen -= fldlen + oldfldlen;
                    wasbreak = (wordlen > 0);
                } else {
                    oldfldlen = fldlen;
                    goto prevfld;
                }
                break;
            } else {
                oldfldlen = 0;
newword:
                fldlen -= wordlen - oldfldlen;
                fldempt = NO;
                i = getword (NO);
                if (i == 2)
                    { pccur = pfcur->curcrt; goto end; } /* Кончились val'ы */
                if (wasbreak)
                    { wasbreak = NO; break; }      /* След.слово-с нов.поля */
                if (i == 1) break;                       /* Новый параграф  */
                if (wordlen > 0) fldlen--;
                if (fldlen <= 0) break;

            }
        }
        oldfldlen = 0;
prevfld:
        pfcur = prevcont (pfcur, papa);
    }

end:
    pcmul = curcol->mult->curcrt; i = 0;
    while (1) {
        pcmul = pcmul->next;
        if (pcmul == NULL || pcmul == pccur) return i;
        i++;
    }
}

int getword (bool first) {
    static bool   goprevval;
    byte        * ptmp;
    register      retcode = 0, remain = 0;

    if (first && lastval == curval && doc->pos >= lastval->val.s.len)
        remain = doc->pos - lastval->val.s.len + 1;

    if (goprevval) {
        goprevval = NO;
        if (lastval->par || lastval->newfld) retcode = 1;
        lastval = lastval->prev;
        if (lastval == curcol->savval) return 2;
        q = lastval->val.s.p + lastval->val.s.len - 1;
    }
    if (lastval->val.s.len == 0)
        { q++; wordlen = 0; goprevval = YES; return retcode; }

    ptmp = q;

    while (* ptmp != ' ') {
        if (ptmp == lastval->val.s.p) {
            wordlen = (int) (q - ptmp) + 1;
            goprevval = YES;
            goto ret;
        }
        ptmp--;
    }

    while (* ptmp == ' ') {
        if (ptmp == lastval->val.s.p) {
            wordlen = (int) (q - ptmp) + 1;
            goprevval = YES;
            goto ret;
        }
        ptmp--;
    }
    wordlen = (int) (q - ptmp) - 1;
ret:
    q = ptmp;
    wordlen += remain;
    return retcode;
}

/*****************************************************************************/
/*             Функции для работы автопереносных полей с базой               */
/*****************************************************************************/

static IDCOL * val2save (FLD * pfld) {
    IDCOL       * pidcol;
    TXTVAL      * ptxtval, * prevval = NULL;
    FLD         * pftmp, * begpar, * endpar;
    byte        * p;
    PRNOPT        prnopt;
    register      vallen, curlen;

    pidcol = utalloc (IDCOL);

    while (1) {
        pftmp = prevcont (pfld, papa);
        if (pftmp == NULL) break;
        pfld = pftmp;
    }

    pfld->opt.par = YES;
    pidcol->first = pfld;
    begpar = pfld;
    prnopt = begpar->prnopt;
    vallen = 0;

    while (1) {
        pfld->opt.done = YES;
        if (pidcol->mult == NULL && pfld->curcrt->opt.multy)
            pidcol->mult = pfld;
        vallen += pfld->val.s.len;
        pftmp = nextcont (pfld, papa);
        if (pftmp == NULL || pftmp->opt.par) {
            endpar = pfld;
            ptxtval = utalloc (TXTVAL);
            ptxtval->par = YES;
            if (prevval == NULL) pidcol->pval = ptxtval;
            else {
                prevval->next = ptxtval;
                ptxtval->prev = prevval;
            }
            prevval = ptxtval;
            ptxtval->prnopt = prnopt;
            if (vallen > 0) {
                ptxtval->prnopt.full = YES;
                p = ptxtval->val.s.p = malloc (vallen + 1);
                ptxtval->val.s.len = vallen;
                while (1) {
                    if (begpar->prnopt.full) {
                        curlen = begpar->val.s.len;
                        utmovabs (p, begpar->val.s.p, curlen, 0);
                        p += curlen;
                        * p++ = ' ';
                    }
                    if (begpar == endpar) {
                        * (p - 1) = EOS;
                        begpar = pftmp;
                        prnopt = begpar->prnopt;
                        vallen = 0;
                        break;
                    }
                    begpar = nextcont (begpar, papa);
                }
            } else ptxtval->prnopt.full = NO;
        }
        if (pftmp == NULL) break;
        pfld = pftmp;
        if (!pfld->prnopt.full) continue;
        if (vallen > 0) vallen++;
    }
ret:
    pidcol->last = pfld;
    ptxtval->next = pidcol->pval;
    pidcol->pval->prev = ptxtval;

    return pidcol;
}

void  uvval2col (bool first) {
    FLD     * pfcur;
    TXTVAL  * pval;

    if (first) {
        pfcur  = curcol->first;
        pval   = curcol->pval;
    } else {
        pfcur = curcol->mult;
        if (curcol->savval == NULL) {
            if (pfcur == NULL) return;
            goto ret;
        }
        pfcur  = nextcont (pfcur, papa);
        pval   = curcol->savval->next;
    }

    while (1) {
        pfcur->val = pval->val;
        pval->val.s.p = NULL;
        pfcur->opt.par = pval->par;
        pfcur->prnopt = pval->prnopt;
        pfcur->opt.done = YES;

        if (pval->next == curcol->pval) { curcol->savval = NULL; goto ret; }
        if (pfcur == curcol->mult) break;
        if (pfcur == curcol->last) { curcol->savval = NULL; return; }
        pval = pval->next;
        pfcur = nextcont (pfcur, papa);
    }
    curcol->savval = pval;
    return;

ret:
    while ((pfcur = nextcont (pfcur, papa)) != NULL) {
        pfcur->prnopt.full = pfcur->opt.par = NO;
        pfcur->opt.done = YES;
    }
    return;
}

void uvfitval (void) {
    FLD     * pfcur = curcol->last;

    if (curcol->mult == NULL || curcol->savval == NULL) return;

    lastval = curcol->pval->prev;

    while (1) {     /* от последнего поля до множественного, не включая его */
        pfcur = multcols (pfcur);
        lastval = lastval->prev;
        if (lastval == curcol->savval) return;
        pfcur = prevcont (pfcur, papa);
    }
}

FLD * multcols (FLD * pfcur) {
    IDCOL   * tmpcol;
    FLD     * pftmp;

    if (pfcur == curcol->mult) {
        dbcrtmul (doc, pfcur->curcrt, YES, NO);
        pfcur = nextcont (pfcur, papa);
        if (pfcur->curcrt->next == NULL) {
            for (tmpcol = firstcol; tmpcol != NULL; tmpcol = tmpcol->next){
                pftmp = nextcont (tmpcol->mult, papa);
                if (tmpcol->mult == tmpcol->last) tmpcol->last = pftmp;
            }
        }
    }
    return pfcur;
}

static int getfldlen (FLD * pfld, PRNOPT * prnopt) {
    int len;

    dbbeglen (doc, pfld, NULL, &len);

    if (doc->opt.for_prn && pfld->opt.visibl) {
        len *= getwidsym (&doc->prnopt);
        len /= getwidsym (prnopt);
    }
    return len;
}

