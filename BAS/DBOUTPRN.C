#include    <sldbas.h>
#include    <slmenu.h>
#include    <slkeyb.h>
#include    <slsymb.def>

int               H_page, Cur_string, Num_page;
IDPRN           * Curidprn;

static ITEM       pitem [] = {
    { " Продолжить ", "ПпGg", NULL },
    { " Прекратить ", "РрHh", NULL }
};
static PAGBRK   * curbrk;
static PRNPAG   * prnpag;
static float      li, pt;
static FLD      * pnext, * phigh;
static PNSWAP   * highswp;
static PNSWAP   * fldswp;
static int        nstr, n;
static int        highpn;
static int        fldpn, hand;
static byte     * Tmp_buf, * For_v2c, * For_txt;
static byte       inibuf [] = { 27, '@', 27, 'x', 0, 27, 'P', 27, '5',
                                27, '-', 0,  27, 'W', 0, 18,  27, 'T',
                                27, 'F', 27, 'H' };
static byte       interval [] = { 27, 51, 0 };
static byte       rn       [] = { 13, 10 };
static byte       ff       [] = { 12 };

int             dbprnstr     (void far *to, void far *from);
static int      note2prn     (void);
static int      outleft      (void);
static int      outlf        (int numlf);
static int      outrn        (int num);
static int      outnote      (int mode);
static int      fmtprn       (int widin, int remain, PRNOPT * prnopt);
static void     test_row     (DOC * doc);
static int      prnfldstr    (DOC * doc);
static int      outprnfld    (DOC * doc, int len);
static int      str2prn      (byte * p, int len);
static int      outbuf       (byte * p, int len, PRNOPT * prnopt);
static int      offprnopt    (PRNOPT * prnopt);
static int      setprnopt    (PRNOPT * prnopt);
static int      outgrafblank (int num);
static int      calcH_page   (void);


bool    dbprnlst  (char * filename) {
    bool        retcode = NO;
    int         _4 = 4, _216 = 216;

    if (!specprn.n_216) { _4 = 2; _216 = 180; }

    li = 25.4 / (float) _216;
    pt = 25.4 / 120;
    hand = -1;
    if ((Tmp_buf = malloc (1024)) == NULL) goto end;
    if ((For_v2c = malloc (1024)) == NULL) goto end;
    if ((For_txt = malloc (1024)) == NULL) goto end;
    retcode = YES;
    if (filename != NULL) hand = utcreate (filename);

    while (1) {
        if (filename == NULL && Curidprn->setprnpag != NULL &&
                                              !Curidprn->setprnpag ()) break;
        Num_page = 0;
        prnpag = &Curidprn->prnpag;
        n = min ((_4 + prnpag->linspac) * 3, _216);
        if (hand == -1) {
            curbrk = prnpag->firstbrk;
            H_page = calcH_page ();
        }
        wndiswrk (0L, 3, " ", " Печатание документа ", " ");

        Curidprn->outprn ();

        wnremwrk ();
        if (filename != NULL) break;
    }

end:
    utclose (hand);

    utfree (&For_txt);
    utfree (&For_v2c);
    utfree (&Tmp_buf);
    return retcode;
}

int    dbprnlstdoc  (DOC * doc, int mode) {
    register    i, j;
    int         total;

    if (hand == -1) {
        if ((mode & PR_BEGNOTE) == PR_BEGNOTE) {
            Cur_string = 0;
            if (outnote (0) > 0) return NO;
            curbrk = prnpag->firstbrk;
            H_page = calcH_page ();
        }
    }

    WORK;
    total = 0;
    phigh = NULL;
    pnext = doc->crtroot->fld_first;
    if (!pnext->opt.visibl) pnext = next (pnext);

    while (1) {
        test_row (doc);

        for (i = 0; i < nstr; i++, highpn++) {
            if (outleft () > 0) return NO;
            total++;
            Cur_string++;
            j = dbprnstr (For_txt, highswp->ptxt + highswp->pn [highpn]);
            if (outbuf (For_txt, j, &doc->prnopt) > 0) return NO;
            if (outrn (n) > 0) return NO;
            if (hand == -1 && Cur_string == H_page && total != doc->total) {
                if (outnote (1) > 0) return NO;
                Cur_string = 0;
                H_page = calcH_page ();
            }
        }
        if (pnext == NULL) break;

        if (outleft () > 0) return NO;
        total++;
        Cur_string++;
        if (prnfldstr (doc) > 0) return NO;
        if (outrn (n) > 0) return NO;
        if (hand == -1 && Cur_string == H_page && total != doc->total) {
            if (outnote (1) > 0) return NO;
            Cur_string = 0;
            H_page = calcH_page ();
        }
    }

    if (hand == -1) {
        if ((mode & PR_ENDNOTE) == PR_ENDNOTE) {
            if (outnote (2) > 0) return NO;
            Cur_string = 0;
        }
    }

    return YES;
}

bool    dboutprn  (DOC * doc, char * filename, void (*preview) (void)) {
    bool        retcode = NO;

    hand = -1;
    if ((Tmp_buf = malloc (1024)) == NULL) goto end;
    if ((For_v2c = malloc (1024)) == NULL) goto end;
    if ((For_txt = malloc (1024)) == NULL) goto end;
    retcode = YES;
    if (filename != NULL) hand = utcreate (filename);


    while (1) {
        if (filename == NULL &&
                 !mnprnpag (&doc->prnpag, &Att_Mnu1, preview, doc->total, YES))
            break;
        wndiswrk (0L, 3, " ", " Печатание документа ", " ");

        prnpag = &doc->prnpag;
        dbprndoc (doc);
        wnremwrk ();
        if (filename != NULL) break;
    }

end:
    utclose (hand);

    utfree (&For_txt);
    utfree (&For_v2c);
    utfree (&Tmp_buf);
    return retcode;
}

int    dbprndoc  (DOC * doc) {
    register    i, j;
    int         ret, numcop = 1, total, space, copspac = 1;
    bool        neednote = YES;
    int         _4 = 4, _216 = 216;

    if (!specprn.n_216) { _4 = 2; _216 = 180; }


    li = 25.4 / (float) _216;
    pt = 25.4 / 120;

    if (hand == -1) {
        Num_page = 0;
        space = n = min ((_4 + prnpag->linspac) * 3, _216);
        numcop  = prnpag->numcop;
        Cur_string  = 0;
        if (prnpag->opt.eq) {
           i = (int) ((float) prnpag->hei / li);
           j = (int) (((float) ((int) ((float) (doc->total * n) * li) +
                                    prnpag->top + prnpag->bot) * numcop) / li);
           if (i < j) {
                mnwarnin (NULL, 0, 4, " ", "Заказанное количество экземпляров",
                                                "не умещается на лист", " ");
                return 1;
           }
           space = (int) ((float) (prnpag->top + prnpag->bot) / li)
                                                          + (i - j) / numcop;
           copspac = 0;
        } else if (prnpag->copspac > 0)
            { copspac = prnpag->copspac; space *= copspac; }
    }
    WORK;
    for (; numcop > 0; numcop--) {
        total = 0;
        phigh = NULL;
        pnext = doc->crtroot->fld_first;
        if (!pnext->opt.visibl) pnext = next (pnext);
        if (hand == -1) {
            if (prnpag->opt.newpage) Num_page = 0;
            if (neednote) {
                if ((ret = outnote (0)) > 0) return ret;
            } else neednote = YES;
            curbrk = prnpag->firstbrk;
            H_page = calcH_page ();
        }
        while (1) {
            test_row (doc);

            for (i = 0; i < nstr; i++, highpn++) {
                if ((ret = outleft ()) > 0) return ret;
                total++;
                Cur_string++;
                j = dbprnstr (For_txt, highswp->ptxt + highswp->pn [highpn]);
                if ((ret = outbuf (For_txt, j, &doc->prnopt)) > 0) return ret;
                if ((ret = outrn (n)) > 0) return ret;
                if (hand == -1 && Cur_string == H_page &&
                                     (total != doc->total || numcop > 1)) {
                    if ((ret = outnote (1)) > 0) return ret;
                    Cur_string = 0;
                    H_page = calcH_page ();
                }
            }
            if (pnext == NULL) break;

            if ((ret = outleft ()) > 0) return ret;
            total++;
            Cur_string++;
            if ((ret = prnfldstr (doc)) > 0) return ret;
            if ((ret = outrn (n)) > 0) return ret;
            if (hand == -1 && Cur_string == H_page &&
                                        (total != doc->total || numcop > 1)) {
                if ((ret = outnote (1)) > 0) return ret;
                Cur_string = 0;
                H_page = calcH_page ();
            }
        }
        if (hand != -1)  return 2;

        if (prnpag->opt.eq || !prnpag->opt.newpage &&
                (!prnpag->opt.full || Cur_string+doc->total+copspac <= H_page)) {
            Cur_string += copspac;
            if (numcop > 1) {
                if ((ret = outlf (space)) > 0) return ret;
                if (Cur_string < H_page)  neednote = NO;
            }
        }
        if (neednote) {
            if ((ret = outnote (2)) > 0) return ret;
            Cur_string = 0;
        }
    }
    if (!neednote && (ret = outnote (2)) > 0) return ret;

    return 1;
}

static int prnfldstr (DOC * doc) {
    FLD     * ptmp;
    register  numbrick = 1, j;
    int       beg = 0, len = 0, endfld, size, ret;
    byte    * q = For_txt, * p;

    p =  fldswp->ptxt + fldswp->pn [fldpn];
    j = dbprnstr (q, p);

    while (1) {
        endfld  =  beg + len;
        p += dbfindbr (p, &beg, &len, numbrick);
        size = beg - endfld;
        ret = outbuf (q + endfld, size, &doc->prnopt); /* Текст между полями */
        if (ret > 0) return ret;

        if ((ret = outprnfld (doc, len)) > 0) return ret;
        ptmp = next (pnext);
        if (ptmp == NULL || ptmp->instr == 1) break;
        numbrick = ptmp->instr - pnext->instr;
        pnext = ptmp;
    }
    endfld  =  beg + len;
    size    =  j - endfld;
    ret = outbuf (q + endfld, size, &doc->prnopt);
    phigh = pnext;
    pnext = ptmp;

    return ret;
}

static int  outprnfld (DOC * doc, int fldlen) {
    PRNOPT      prnopt = pnext->prnopt;
    int         vallen, ret, widout, widin, lenout, lenin, l = 0, r;
    int         jst = pnext->prnopt.jst;
    byte      * q;

    q = For_v2c;

    vallen = dbvaltoc (doc, pnext, &q, fldlen);

    if (hand == -1) {
        widout = getwidsym (&doc->prnopt);
        widin  = getwidsym (&prnopt);
        lenout = widout * fldlen;
lab:
        lenin  = widin * vallen;
        if (lenin > lenout) { vallen = lenout / widin; goto lab; }
        if (jst == UT_FORMAT && pnext->val.s.word_1 > 0)
            return fmtprn (widin, lenout - lenin, &prnopt);
        fldlen = lenout / widin;
        r      = lenout % widin;
        if (jst != UT_FORMAT) {
            if (jst & UT_CENTER)     l = r / 2;
            else if (jst & UT_RIGHT) l = r;
        }
        r -= l;
        if ((ret = outgrafblank (l)) > 0)  return ret;
    }
    if (jst == UT_FORMAT) {
        if (pnext->val.s.word_1 > 0)
            dbfmtscr (Tmp_buf, q,
                        pnext->val.s.blank / (pnext->val.s.word_1),
                                pnext->val.s.blank % (pnext->val.s.word_1));
        else { jst = UT_LEFT; goto jststr; }
    } else
jststr:
        utstrjst ((char *) Tmp_buf, (char *) q, vallen,
                                    (dbgetidf (doc, pnext))->sym, fldlen, jst);

    if ((ret = outbuf (Tmp_buf, fldlen, &prnopt)) > 0) return ret;

    if (hand == -1) ret = outgrafblank (r);

    return ret;
}

static int      fmtprn (int widin, int remain, PRNOPT * prnopt) {
    register    above, remabove;
    int         ret, num;
    byte      * pbeg, * p;

    p = pbeg = pnext->val.s.p;
    above    = remain / (pnext->val.s.word_1);
    remabove = remain % (pnext->val.s.word_1);
    while (* p == ' ') p++;
    if ((ret = outgrafblank ((int) (p - pbeg) * widin)) > 0) return ret;
    while (1) {
        pbeg = p;
        while (* p != ' ' && * p != '\0') p++;
        if ((ret = outbuf (pbeg, (int) (p - pbeg), prnopt)) > 0) return ret;
        if (* p == '\0') return 0;
        num = widin + above;
        if (remabove > 0) { num++; remabove--; }
        if ((ret = outgrafblank (num)) > 0) return ret;
        p++;
    }
}

static void     test_row (DOC * doc) {
    register    lowpn;
    IDLIN     * pidd;

    highswp = &doc->o;

    if (pnext != NULL) {
        if (pnext->idc == 0) {
            lowpn  = dbgetpno (doc, pnext);
            if (phigh != NULL) {
                highpn = dbgetpno (doc, phigh) + 1;
                nstr = lowpn - highpn;
            } else {
                nstr   = lowpn;
                highpn = 0;
            }
            fldswp = highswp;
            fldpn  = highpn + nstr;
        } else {
            pidd = dbgetidd (doc, pnext);
            nstr = pidd->num - 1;
            highpn  = pidd->npn;
            highswp = &doc->d;
            fldpn = highpn + nstr;
            fldswp = highswp;
        }
    } else {
        nstr = doc->total - doc->lastline - 1;
        highpn = dbgetpno (doc, phigh) + 1;
    }
    return;
}

static  int  str2prn (byte * p, int len) {
    register    i, ret;

    for (i = 0; i < len; i++) {
cont:
        utch2prn (p [i]);
        if ((ret = mnharder (pitem, 2)) == -1) continue;
        if (ret == 0) goto cont;
        return ret;
    }
    return 0;
}

static  int  outbuf (byte * p, int len, PRNOPT * prnopt) {
    register    ret;
    long        offset;

    if (kbvfysym (Esc)) {
        ret = mnwarnin (pitem, 2, 3, " ", "Прерывание по \"Esc\"", " ");
        if (ret > 0) return ret;
    }

    if (hand == -1) {
        if ((ret = setprnopt (prnopt)) > 0) return ret;
        if ((ret = str2prn (p, len)) > 0)   return ret;
        return (offprnopt (prnopt));
    }
    offset = utlseek (hand, 0L, UT_CUR);
    do {
        utlseek (hand, offset, UT_BEG);
        utwrite (hand, p, len);
        ret = mnharder (pitem, 2);
    } while (ret == 0);

    return ret;
}

static  int  setprnopt (PRNOPT * prnopt) {
    byte   * q = For_v2c;

    if (prnopt->nlq != 0) {
        * q++ = 27; * q++ = 'x'; * q++ = 1;
        * q++ = 27; * q++ = 'k'; * q++ = prnopt->nlq - 1;
    }
    if (prnopt->size != 0) {
        if (prnopt->size == 1)  { * q++ = 27; * q++ = 'W'; * q++ = 1; }
        else                      * q++ = 15;                /* SI */
    }
    if (prnopt->elite)  { * q++ = 27;  * q++ = 'M'; }     /* Esc M */
    if (prnopt->italic) { * q++ = 27;  * q++ = '4'; }     /* Esc 4 */
    if (prnopt->under)  { * q++ = 27;  * q++ = 45; * q++ = 1; }   /* Esc - 1 */


    if (prnopt->script != 0)
        { * q++ = 27; * q++ = 'S'; * q++ = prnopt->script - 1; }

    if (prnopt->bold > 0) {
        * q++ = 27;
        if (prnopt->bold == 1) * q++ = 'G';
        else                   * q++ = 'E';
    }
    return (str2prn (For_v2c, (int) (q - For_v2c)));
}

static  int  offprnopt (PRNOPT * prnopt) {
    byte   * q = For_v2c;

    if (prnopt->nlq != 0) { * q++ = 27; * q++ = 'x'; * q++ = 0; }
    if (prnopt->size != 0) {
        if (prnopt->size == 1)  { * q++ = 27; * q++ = 'W'; * q++ = 0; }
        else                      * q++ = 18;                /* DC2 */
    }
    if (prnopt->elite)  { * q++ = 27;  * q++ = 'P'; }     /* Esc P */
    if (prnopt->italic) { * q++ = 27;  * q++ = '5'; }     /* Esc 5 */
    if (prnopt->under)  { * q++ = 27; * q++ = 45; * q++ = 0; }   /* Esc - 0 */
    if (prnopt->script != 0) { * q++ = 27;  * q++ = 'T'; }
    if (prnopt->bold > 0) {
        * q++ = 27;
        if (prnopt->bold == 1) * q++ = 'H';
        else                   * q++ = 'F';
    }

    return (str2prn (For_v2c, (int) (q - For_v2c)));
}

int getwidsym (PRNOPT * prnopt) {
    register wid;

    if (prnopt->elite) wid = 10;
    else               wid = 12;

    if (prnopt->size == 1) return (wid * 2);
    if (prnopt->size == 2 && prnopt->nlq == 0) return (wid / 2 + 1);
    return wid;
}

static  int outgrafblank (int num) {
    byte      * p = Tmp_buf;
    register    ret;

    if (num == 0) return 0;
    * p++ = 27; * p++ = 'Y'; * p++ = (byte) num % 256; * p++ = num / 256;
    if ((ret = str2prn (Tmp_buf, 4)) > 0) return ret;
    for (; num > 0; num--) {
cont:
        utch2prn (0);
        if ((ret = mnharder (pitem, 2)) == -1) continue;
        if (ret == 0) goto cont;
        return ret;
    }
    return 0;
}

static  int outnote (int mode) {
    int         sizetxt, i, ret, tonote = 0;
    bool        note;

    if (mode > 0) {
        note = (Num_page > 1 || prnpag->opt.note1st);
        if (prnpag->opt.note == 2 && note) {
            sizetxt = (int) ((float) (Cur_string *  n) * li);
            i = prnpag->hei - prnpag->top - sizetxt - prnpag->tonote;
            if ((ret = outlf ((int) ((float) i / li))) > 0) return ret;
            if ((ret = note2prn ()) > 0) return ret;
        }
        if (prnpag->opt.ff && (ret = str2prn (ff, 1)) > 0) return ret;
    }
    if (mode < 2) {
        if (mnwarnin (NULL, 0, 3, " ", "Вставьте бумагу", " ") == -1)
            return 1;
        Num_page++;
        if (mode == 0 && (ret = str2prn (inibuf, 22))  > 0) return ret;

        note = (Num_page > 1 || prnpag->opt.note1st);
        if (prnpag->opt.note == 1 && note) {
            tonote = max (prnpag->tonote - specprn.autopull, 0);
            if ((ret = outlf ((int) ((float) tonote / li))) > 0) return ret;
            if ((ret = note2prn ()) > 0) return ret;
        }
        if ((ret = outlf((int)((float)(max(prnpag->top - tonote -
                        specprn.autopull, 0)) / li))) > 0) return ret;
    }
    return 0;
}

static  int  outlf (int numlf) {
    register ret, remain = numlf % n;

    for (numlf = numlf / n; numlf > 0; numlf--)
        if ((ret = outrn (n)) > 0) return ret;
    if (remain > 0) return (outrn (remain));
    return 0;
}

static int note2prn (void) {
    int     ret, jst = UT_LEFT, len, numlen, vallen;
    byte    pnum [7];

    len = (int) ((float) (prnpag->wid - prnpag->right - prnpag->left) / pt) /
                                                   getwidsym (&prnpag->prnopt);
    vallen = strlen (prnpag->pnote);
    numlen =  utl2crdx ((long) Num_page, pnum, 0, 10);
    if (len < vallen + numlen + 1) vallen = len - numlen - 1;

    if (prnpag->opt.mirror && Num_page % 2 == 0) jst = UT_RIGHT;

    if ((ret = outleft ()) > 0) return ret;
    utstrjst ((char *) Tmp_buf, (char *) prnpag->pnote, vallen, ' ', len, jst);

    if (prnpag->opt.number) {
        if (jst == UT_RIGHT) utmovabs (Tmp_buf, pnum, numlen, 0);
        else  utmovabs (Tmp_buf + len - numlen, pnum, numlen, 0);
    }
    if ((ret = outbuf (Tmp_buf, len, &prnpag->prnopt)) > 0) return ret;
    return (outrn ((prnpag->opt.note == 1) ? 1 : n));
}

static  int  outrn (int num) {
    int     ret;

    if (hand == -1) {
        interval [2] = num;
        if ((ret = str2prn (interval, 3)) > 0) return ret;
        return (str2prn (rn, 2));
    }
    return (outbuf (rn, 2, NULL));
}

static int  outleft (void) {
    int left = prnpag->left;

    if (hand != -1) return 0;
    if (prnpag->opt.mirror && Num_page % 2 == 0) left = prnpag->right;
    return (outgrafblank ((int) ((float) left / pt)));
}

static int calcH_page (void) {
    int h_page;

    if (prnpag->opt.brk && (prnpag->opt.newpage || prnpag->numcop == 1)) {
        h_page = (int) (curbrk->next->line - curbrk->line);
        curbrk = curbrk->next;
    } else h_page = (int) ((float) (prnpag->hei - prnpag->top - prnpag->bot) /
                                                              ((float) n * li));
    return h_page;
}

