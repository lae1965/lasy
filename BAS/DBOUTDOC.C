#include    <sldbas.h>

static  byte  * hightxt, * fldtxt;
static  int   * ppn, npn, nstr;

void dboutstr (CELLSC far *to, void far *from, int visicol, int w, char attr);
static void    test_row  (DOC * doc, FLD * high, FLD  * low);
static FLD   * outfldstr (DOC * doc, FLD * pfld, int row, byte * p, bool high);
static void    outbrk    (DOC * doc, CELLSC * pdata);

void    dboutdoc  (DOC * doc) {
    FLD      * pcur = doc->cur->pfld, *ptmp, *firstcur = pcur, *lastcur = pcur;
    FLD      * ptmp2;
    register   nrow = doc->cur->row, i;
    int        docrow = nrow, visicol = doc->cur->visicol;
    int        H = doc->pwin->img.dim.h;
    int        W = doc->pwin->img.dim.w;
    byte       txtatt = doc->pwin->attr.text;
    CELLSC   * pdata = doc->pwin->img.pdata;
    char far * pbuf, * pscreen;

    i = dbgetpno (doc, pcur) - nrow;

    pbuf    = (char far *) doc->pwin->img.pdata;
    pscreen = wnviptrl (doc->pwin->where_shown.row, doc->pwin->where_shown.col);


    /*************  Вывод строки с текущим полем ********************/
    /************* Само текущее поле не выводится *******************/
    /************* Текущим полем занимается редактор поля ***********/

    while (firstcur->instr != 1) firstcur = firstcur->prev;
    while (1) {
        ptmp = next (lastcur);
        if (lastcur->instr >= ptmp->instr || ptmp == NULL) break;
        lastcur = ptmp;
    }
    /*************  Вывод строк над текущим полем, включая его **************/
    ptmp = firstcur;
    nrow = docrow;
    while (nrow >= 0) {
        ptmp2 = prev (ptmp);
        test_row (doc, ptmp2, ptmp);

        if (nrow >= H) {
            if (nrow - nstr >= H) { nrow -= nstr + 1; nstr = 0; }
            else { nstr -= nrow - H; nrow =  H - 1; }
        } else {
            dboutstr (pdata + W * nrow, fldtxt, visicol, W, txtatt);
            ptmp = outfldstr (doc, ptmp, nrow, fldtxt, YES);
            nrow--;
        }
        for (i = nstr - 1; nrow >= 0 && i >= 0; nrow--, i--)
            dboutstr (pdata + W*nrow, hightxt + ppn[npn+i], visicol, W, txtatt);

        if (ptmp2 != NULL) while (ptmp2->instr != 1) ptmp2 = ptmp2->prev;
        ptmp = ptmp2;
    }
    /*************  Вывод строк под текущим полем ********************/
    ptmp = lastcur;
    nrow = docrow + 1;
    while (1) {
        ptmp2 = next (ptmp);
        test_row (doc, ptmp, ptmp2);

        if (nrow < 0) { i = -nrow; nrow += nstr; nrow = min (nrow, 0); }
        else            i =  0;

        for (; nrow < H && i < nstr; nrow++, i++)
            dboutstr (pdata + W*nrow, hightxt + ppn[npn+i], visicol, W, txtatt);

        if (nrow >= H || ptmp2 == NULL) break;
        ptmp = ptmp2;

        if (nrow >= 0) {
            dboutstr (pdata + W * nrow, fldtxt, visicol, W, txtatt);
            ptmp = outfldstr (doc, ptmp, nrow, fldtxt, NO);
        }
        nrow++;
    }
    wnmovpca (pdata + nrow * W, ' ', (H - nrow) * W, txtatt);
    if (doc->opt.for_prn) outbrk (doc, pdata);
    if (* Db_curidedt->updscreen == NULL)
        wnvicopy (&pbuf, &pscreen, H, W, NUM_COLS * 2, 0, Cmd[1]);
    else (* Db_curidedt->updscreen) (doc);
    return;
}

static FLD  * outfldstr (DOC * doc, FLD * pfld, int row, byte * p, bool high) {
    FLD     * firstfld, * ptmp;
    register  numbrick = 1;
    int       beg = 0, len = 0;

    while (pfld->instr != 1) pfld = pfld->prev;   /* Первое поле строки */
    firstfld = pfld;
    while (1) {
        p += dbfindbr (p, &beg, &len, numbrick);
        dboutfld (doc, pfld, row, beg, len, NO, NO);
        if ((ptmp = next (pfld)) == NULL || ptmp->instr == 1) break;
        numbrick = ptmp->instr - pfld->instr;
        pfld = ptmp;
    }
    if (high) return firstfld;
    return  pfld;
}

void  dboutfld (DOC * doc, FLD * pfld, int row, int beg, int len,
                                                    bool mistake, bool visi) {
    IDFLD     * pidf;
    register    i = beg;
    int         W = doc->pwin->img.dim.w, visicol = doc->cur->visicol;
    ATTRIB    * attr = &doc->pwin->attr;
    int         jst = pfld->prnopt.jst;
    int         space, left = 0, right = 0;
    int         vallen, fldlen, r, l;
    byte      * q, att, sym;
    char far  * pbuf, * pscreen;
    CELLSC    * pim, * pdata;

    pidf = dbgetidf (doc, pfld);
    sym = pidf->sym;
    fldlen = len;
    if (!dbcor_bl (visicol, W, &beg, &len)) return;

    pim = pdata = doc->pwin->img.pdata + row * W + beg;

    q = Db_pwrk;
    if (doc->opt.for_prn || doc->opt.for_lst) att = attr->text;
    else if (pfld == doc->cur->pfld) {
        if (mistake)            att = attr->err;
        else if (pfld->opt.par) att = attr->bold;
        else                    att = attr->afld;
        if (Db_edit && !pfld->opt.noedit) {
            jst = 0;
            q = doc->pwrk;
            vallen = doc->cur->len;
        }
    } else {
        if (pfld->opt.protect)  att = attr->prot;
        else if (pfld->opt.par) att = attr->nice;
        else                    att = attr->nfld;
    }
    if (* Db_curidedt->updfld != NULL) Db_curidedt->updfld (doc, pfld, &att);

    if (pfld->prnopt.jst == UT_FORMAT && pfld->val.s.word_1 > 0) {
        if (doc->cur->pfld != pfld || !Db_edit)
            vallen = dbfmtscr (q, pfld->val.s.p,
                        pfld->val.s.blank / (pfld->val.s.word_1),
                                pfld->val.s.blank % (pfld->val.s.word_1));
    } else if (doc->cur->pfld != pfld || !Db_edit || pfld->opt.noedit)
        vallen = dbvaltoc (doc, pfld, &q, fldlen);
    l = beg + visicol - i;
    r = fldlen - l - len;

    if (vallen > 0 && (jst & UT_SGN_BEG) &&
                                    (* q == '+' || * q == '-' || * q == ' ')) {
        if (l > 0) l--;
        else { pim->ch  = * q; pim->at = att; pim++; }
        vallen--; fldlen--; q++;
    }

    vallen = min (vallen, fldlen);
    space = fldlen - vallen;

    if (jst == UT_FORMAT) right = space;
    else if (jst & UT_RIGHT)  left  = space;
    else if (jst & UT_CENTER)
        { left = space / 2; right = fldlen - left - vallen; }
    else                  right = space;

    left -= l; right -= r;
    i = min (left, len);

    if (left > 0)   { wnmovpca (pim, sym, i, att); pim += i; }
    else            { q -= left; vallen += left; }
    if (vallen < 0)   right += vallen;
    if (right  < 0)   vallen += right;
    if (vallen > 0) { wnmovpsa (pim, q, vallen, att);   pim += vallen; }
    if (right  > 0)   wnmovpca (pim, sym, right, att);

    if (visi) {
        pbuf = (char far *) pdata;
        pscreen = wnviptrl (doc->pwin->where_shown.row + row,
                                            doc->pwin->where_shown.col + beg);
        wnvicopy (&pbuf, &pscreen, 1, len, NUM_COLS*2, 0, Cmd[1]);
    }
    return;
}

static void     test_row (DOC * doc, FLD * high, FLD  * low) {
    register    highpn, lowpn;
    IDLIN     * pidd;

    ppn = doc->o.pn;
    hightxt = doc->o.ptxt;

    if (low != NULL) {
        if (low->idc == 0) {
            lowpn  = dbgetpno (doc, low);
            if (high != NULL) {
                highpn = dbgetpno (doc, high);
                nstr = lowpn - highpn - 1;
                npn = highpn + 1;
            } else {
                nstr = lowpn;
                npn  = 0;
            }
        } else {
            pidd = dbgetidd (doc, low);
            ppn = doc->d.pn;
            hightxt = doc->d.ptxt;
            nstr = pidd->num - 1;
            npn = pidd->npn;
        }
        fldtxt = hightxt + ppn [npn + nstr];
    } else {
        nstr = doc->total - doc->lastline - 1;
        npn =  dbgetpno (doc, high) + 1;
    }
    return;
}

static void outbrk (DOC * doc, CELLSC * pdata) {
    PAGBRK    * curbrk = doc->prnpag.firstbrk;
    long        curline = doc->view.line - doc->view.row;
    int         H = doc->pwin->img.dim.h;
    int         W = doc->pwin->img.dim.w;
    byte        att = doc->pwin->attr.text, fore, back;
    byte        attfix;
    register    i;

    back   = wnscback (att);
    fore   = wnscfore (att);
    att    = wnscattr (fore, back);
    attfix = wnscattr (SL_BLACK, SL_WHITE|SL_INTENSITY);

    while (curline > curbrk->line) curbrk = curbrk->next;

    for (i = 0; i < H; i++, curline++) {
        if (curline == curbrk->line) {
            wnmovpa (pdata + W * i, W, (curbrk->fix) ? attfix : att);
            curbrk = curbrk->next;
        }
    }
    return;
}

