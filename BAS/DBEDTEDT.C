#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>

static BORDER mnbord = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };
static ITEM   jstitm [] = {
    { " Влево      ", "ЛлKk", NULL },
    { " Вправо     ", "ПпGg", NULL },
    { " По центру  ", "ЦцWw", NULL },
    { " Равномерно ", "РрHh", NULL }
};

static bool     bsdelvfy    (bool bs);

void dbed_edt (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    int       b, l;
    char    * pwrk = (char *) doc->pwrk;

    if (!Db_edit) { utalarm (); return; }
    b = cur->beg; l = cur->len;
    dbcor_bl (cur->visicol, doc->pwin->img.dim.w, &b, &l);
    switch (Kb_sym) {
        case Home:   doc->pos = 0;    break;
        case End:
            doc->pos = cur->len - 1;
            if (pwrk [doc->pos] != ' ') break;
            while (doc->pos != 0 && pwrk [doc->pos - 1] == ' ')
                doc->pos--;
            break;
        case CtrlLeft:
            if (doc->pos > 0) {
                while (pwrk[doc->pos-1] == ' ' && doc->pos > 0) doc->pos--;
                while (pwrk[doc->pos-1] != ' ' && doc->pos > 0) doc->pos--;
            } else utalarm ();
            break;
        case CtrlRight:
            if (doc->pos < cur->len) {
                while (pwrk[doc->pos] != ' ' && doc->pos < cur->len-1)
                    doc->pos++;
                while (pwrk[doc->pos] == ' ' && doc->pos < cur->len-1)
                    doc->pos++;
                while (pwrk [doc->pos] == ' ' && doc->pos > 0) doc->pos--;
            } else utalarm ();
            break;
        case Ins:   doc->opt.insert = !doc->opt.insert;  return;
        case Enter:
        case GrayEnter:        dbed_ent (NO);        return;
        case CtrlEnter:
        case GrayCtrlEnter:    dbed_ent (YES);       return;
        default:
            if (Kb_sym == CtrlY && cur->pfld->opt.wrap) { utalarm (); return; }
            switch (Kb_sym) {
                case Del:  delsym ();     break;
                case BS:   bssym ();      break;
                case CtrlY:
                    doc->pos = 0;
                case CtrlK:
                    utmovsc  (&pwrk [doc->pos], ' ', cur->len - doc->pos);
                    break;
                default:
                    if (Kb_ch < 32) { utalarm (); return; }
                    doc->opt.change = cur->pfld->opt.change = YES;
                    if (dbsetsym ())
                        { doc->opt.changpos = NO; dbed_ver (YES); return; }
                    if (doc->pos >= cur->len) {
                        doc->pos = cur->len - 1;
                        if (!doc->opt.autojump || next (cur->pfld) == NULL)
                            { utalarm (); break; }
                        if (Db_curidedt->fldwrk != NULL)
                            (* Db_curidedt->fldwrk) ();
                        if (!dbed_c2v ()) return;
                        if (!dbktab (NO)) { /* Переход невозможен */
                            { currestore (); return; }
                        }
                        setnewfld ();
                        return;
                    }
                    break;
            }
            break;
    }
    doc->opt.change = cur->pfld->opt.change = YES;
    jstmrk ();
    return;
}

void dbed_clb (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    TXTVAL  * ptxtval;
    byte    * p;
    bool      first;

    if (!cur->pfld->opt.wrap) { utalarm (); return; }
    if ((Db_Err = dbctoval (doc, cur->pfld, doc->pwrk, &cur->pfld->val)) > 1)
        { utalarm (); return; }

    first = (cur->pfld->prnopt.jst == UT_FORMAT);

    dboutcol (doc, cur->pfld, NO);

    ptxtval = curval;
    while (!ptxtval->par) ptxtval = ptxtval->prev;

    while (1) {
        if (ptxtval->val.s.len > 0) {
            p = malloc (ptxtval->val.s.len + 1);
            ptxtval->val.s.len = dbclrbln (p, ptxtval->val.s.p, first);
            free (ptxtval->val.s.p);
            ptxtval->val.s.p = p;
        }
        ptxtval = ptxtval->next;
        if (ptxtval->par) break;
        first = NO;
    }
    fmtcol ();

    return;
}

bool     dbsetsym (void) {
    DOC       * doc = Cur_Edt;
    DOCEDT    * cur = doc->cur;
    FLD       * pfld = cur->pfld;
    register    pos = doc->pos, fldlen;
    byte      * pwrk = doc->pwrk, lastch;
    bool        gonextfld = NO, insert = doc->opt.insert;

    fldlen = cur->len;
    lastch = pwrk [fldlen - 1];
    dbctoval (doc, pfld, pwrk, &pfld->val);

    if (insert) utmovabs (&pwrk [fldlen-1], &pwrk [fldlen-2], fldlen-pos-1, 1);
    pwrk [pos] = (char) Kb_ch;
    pos++;
    doc->pos = pos;
    if (!pfld->opt.wrap) return NO;
    if (pfld->prnopt.full) {
        if (pos < fldlen && Kb_ch != ' ' && (!insert || lastch == ' '))
            goto ret;
        if (!insert && pos == fldlen)
            { doc->pos = 0; gonextfld = YES; goto ret; }
    }
    pwrk [fldlen] = lastch;
    pwrk [fldlen + 1] = EOS;

    dbctoval (doc, pfld, pwrk, &pfld->val);
    if (!pfld->prnopt.full) return NO;
    dboutcol (doc, pfld, NO);
    dbprpval (fldlen);
    fmtcol ();
ret:
    return gonextfld;
}

void     bssym (void) {
    DOC       * doc = Cur_Edt;
    DOCEDT    * cur = doc->cur;
    FLD       * pfld = cur->pfld;
    byte      * pwrk = doc->pwrk;
    register    pos = doc->pos;
    int         fldlen = cur->len;
    bool        insert = doc->opt.insert, goprevfld = NO;

    if (pos > 0) {
        if (insert) utmovabs (&pwrk [pos-1], &pwrk [pos], cur->len-pos+1, 0);
        else  pwrk [pos - 1] = ' ';
        pos--;
    } else goprevfld = YES;
    doc->pos = pos;

    if (!pfld->opt.wrap) return;

    dbctoval (doc, pfld, pwrk, &pfld->val);
    if (!goprevfld && !bsdelvfy (YES)) return;
    dboutcol (doc, pfld, NO);

    if (goprevfld && curval != firstcol->pval) {
        curval->par = NO;
        curval = curval->prev;
        pfld = prevcont (pfld, dbgetpapa (pfld));
        dbbeglen (doc, pfld, NULL, &fldlen);
        doc->pos = (curval->val.s.len > 0) ? curval->val.s.len + 1 : 0;
        if (doc->pos >= fldlen) doc->pos = fldlen - 1;
        if (curval->val.s.len == fldlen) curval->val.s.len--;
    }
    dbprpval (fldlen);
    fmtcol ();
    return;
}

void     delsym (void) {
    DOC       * doc = Cur_Edt;
    FLD       * pfld = doc->cur->pfld, * pftmp;
    byte      * pwrk = doc->pwrk;
    register    vallen, pos = doc->pos;
    int         fldlen = doc->cur->len;

    dbctoval (doc, pfld, pwrk, &pfld->val); /* для запоминания длины */
    vallen = pfld->val.s.len;
    utmovabs (&pwrk [pos], &pwrk [pos + 1], fldlen - pos, 0);

    if (!pfld->opt.wrap) return;

    dbctoval (doc, pfld, pwrk, &pfld->val); /* для выяснения, пустое ли поле */
    if (vallen > 0 && !pfld->prnopt.full) return;
    if (pos < vallen && pos >= pfld->val.s.len &&
        ((pftmp = nextcont (pfld, dbgetpapa (pfld))) == NULL || pftmp->opt.par))
            return;

    if (!bsdelvfy (NO)) goto ret;
    dboutcol (doc, pfld, NO);
    dbprpval (fldlen);
    fmtcol ();
ret:
    doc->opt.needout = YES;
    return;
}

static bool bsdelvfy (bool bs) {
    DOC     * doc = Cur_Edt;
    FLD     * pfld, * pfldold = doc->cur->pfld;
    byte    * p;
    int       pos = doc->pos;

    pfld = pfldold = doc->cur->pfld;

    if (!pfld->prnopt.full || pos - bs >= pfld->val.s.len) {
        pfld = nextcont (pfld, dbgetpapa (pfld));
        if (pfld == NULL || bs && pfld->opt.par) return NO;
        pfld->opt.par = NO;
        if (pfld->prnopt.full) {
            p = pfld->val.s.p;
            while (* p == ' ') p++;
            while (* p != ' ' && * p != EOS) p++;
            if (doc->cur->len - pos < (int) (p - pfld->val.s.p) + bs)
                return NO;
            utmovabs (&doc->pwrk[pos+bs], pfld->val.s.p, pfld->val.s.len+1, 0);
            dbctoval (doc, pfldold, doc->pwrk, &pfldold->val);
        } else if (bs && !pfld->opt.par) return NO;
        utfree (&pfld->val.s.p);
        pfld->val.s.len = 0;
        pfld->prnopt.full = NO;

    }
    return YES;
}

void dbed_par (void) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;
    FLD         * pfld = cur->pfld;

    if (!pfld->opt.wrap)  { utalarm (); return; }

    pfld->opt.par = !pfld->opt.par;

    dbctoval (doc, pfld, doc->pwrk, &pfld->val);
    dboutcol (doc, pfld, NO);
    fmtcol ();

    return;
}

void dbed_jst (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pfld = cur->pfld;
    MENU    * pmnu;
    register  i, numitm = 3;

    if (pfld->opt.wrap) numitm = 4;
    pmnu = mncrestd (jstitm, numitm, MN_VERT, 0, 0, &mnbord, &Att_Mnu1);
    mnsetcur (pmnu, pfld->prnopt.jst);
    mndsplay (pmnu, WN_CENTER, WN_CENTER);
    if ((i = mnreadop (pmnu, 0)) >= 0) dbsetjst (pfld, i);
    mndstroy (pmnu);

    if (i == UT_FORMAT) {
        dbctoval (doc, pfld, doc->pwrk, &pfld->val);
        dboutcol (doc, pfld, NO);
        fmtcol ();
    }
    doc->opt.change = YES;
    return;
}

void dbed_cdt (void) {
    FLD  * pfld = Cur_Edt->cur->pfld;

    if ((dbgetidf (Cur_Edt, pfld))->valtype != SL_DATE)
        { utalarm (); return; }
    pfld->val.l = utdtoday ();
    pfld->prnopt.full = YES;
    Cur_Edt->opt.change = pfld->opt.change = YES;
    val2pwrk ();
    jstmrk ();
    dbed_c2v ();
    return;
}

void dbed_ent (bool ctrl) {
    DOC       * doc = Cur_Edt;
    FLD       * pfld = doc->cur->pfld;
    byte      * pwrk = doc->pwrk, * p;
    register    pos = doc->pos, len = 0;

    if (doc->doctype == DB_QRY) { utalarm (); return; }
    if (!pfld->opt.wrap) { dbed_c2v (); return; }
    dbctoval (doc, pfld, pwrk, &pfld->val);
    dboutcol (doc, pfld, NO);

    if (!ctrl && (pos < curval->val.s.len)) {
        p = curval->val.s.p + pos;
        len = curval->val.s.len - pos;
    }
    curval = valbreak (curval, p, len);
    curval->par = YES;
    fmtcol ();
    return;
}

void fmtcol (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    register  i;

    i = dbfmtcol (YES);
    cur->pfld = doc->curfld;
    dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
    val2pwrk ();
    doc->opt.needout = YES;
    rowlinecorr (i);
    cur2old ();
    return;
}

