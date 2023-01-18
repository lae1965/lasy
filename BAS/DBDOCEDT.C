/*******************************************************************************
    Функция dbdocedt осуществляет навигацию и редактирование документов.

    bool dbdocedt (DOC * doc, IDEDT * pidedt, bool edit, bool top);

        DOC * doc            - Указатель на редактируемый документ;
        bool edit            - YES - редактирование, NO - просмотр;
        bool top             - YES - верхний уровень рекурсии;
        int (*ed_fn) (void)  - Указатель на функцию, осуществляющую
                               обработку пользовательских клавиш.
                               Кода возврата этой функции могут принимать
                               следующие значения:
                                  0 - продолжить нормальную работу dbdocedt;
                                  1 - переход на начальные установки документа;
                                  2 - выход из редактирования с кодом YES;
                                 -1 - выход из редактирования с кодом NO;

        Код возврата из dbdocedt (YES/NO) введен для того, чтобы дать понять
        вызывающей функции, по какой клавише произошел выход.

        Клавиши, зарезервированные dbdocedt, см. в функциях dbed_nav и dbed_edt
        в этом же файле.

        Для выведения подсказки по клавишам необходимо:
            - иметь окно шаблона подсказки - Db_phlp;
            - иметь функцию вывода подсказки в окно Db_phlp с названием dbedkhlp,
              которая вызывается из kbgetkey;

*******************************************************************************/

#include    <sldbas.h>
#include    <slmous.h>
#include    <alloc.h>
#include    <slkeyb.h>
#include    <slsymb.def>


int      Db_MousX, Db_MousY;
bool     Db_MousMove;
bool     Db_main_edit, Db_edit;

static  DOC       * docto;
static  ITEM        noyes[] = {
        { " Нет ",          "НнYy",  NULL },
        { " Да ",           "ДдLl",  NULL }
};
static char * cdfkey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
"F5  Изменить  F6/F8  Множ. /  Enter  Выбор  Alt/CtrlF4  Сортировка  Esc  Выход",
"XXX          XXXXXXX           XXXXXXX       XXXXXXXXXXXX            XXXXX      "
};

static void     getvisicol      (void);
static int      vfydict         (void);
static int      ed_dict         (void);
static void     setnewcod       (void);

bool dbdocedt (DOC * doc, IDEDT * pidedt, bool edit, bool top) {
    DOC      * savdoc = Cur_Edt;
    IDEDT    * savidedt = Db_curidedt;
    DOCEDT   * old, * cur, * savcur, * savold;
    int        event, savedit, savmainedit;
    bool       (far * _Cdecl old_msfunc)();
    bool       ret = YES;
    int        H = doc->pwin->img.dim.h, Wrow = doc->pwin->where_shown.row;
    int        W = doc->pwin->img.dim.w, Wcol = doc->pwin->where_shown.col;
    int        b, l, j;

    Cur_Edt = doc;
    Db_curidedt = pidedt;
    savcur = doc->cur;
    savold = doc->old;
    savmainedit = Db_main_edit;
    savedit     = Db_edit;
    if ((doc->cur = utalloc (DOCEDT)) == NULL) { ret = NO; goto edtret; }
    cur = doc->cur;
    if ((doc->old = utalloc (DOCEDT)) == NULL) { ret = NO; goto edtret; }
    old = doc->old;

    doc->pos = 0;
    doc->opt.hide = NO;
    doc->opt.autojump = doc->opt.nulpos = doc->opt.insert = YES;
    doc->opt.single   = 1;

    event = MS_LEFT_PRESS|MS_RIGHT_RELEAS|MS_RIGHT_PRESS|MS_MOVE;
    old_msfunc = msshadow (&event, dbedmous);
    msgetpos (&b, &l, &j);
    dbedmous (0, 0, l, b, 0, 0);
    doc->opt.change = NO;
    Db_main_edit = edit;
newdoc:
    if (!dbnewdoc ()) { ret = NO; goto edtret; }

    if (doc->doctype == DB_DICT && savdoc->doctype != DB_QRY &&
                    (dbgetidf (savdoc, savdoc->curfld))->valtype == SL_CDF) {
        b = savdoc->curfld->val.i;
        if (b > 0) while (b != doc->cur->pfld->next->val.i) {
            if (doc->cur->pfld->curcrt->next == NULL) break;
            dbed_tab (NO);
        }
    }

    if (Db_curidedt->go2firstfld != NULL) (* Db_curidedt->go2firstfld) ();

    while (1) {
        msunhide ();
        if (!Cur_Edt->opt.noflush) kbflushb ();
        if (Curlisting == NULL) wnscrbar (doc->pwin, YES,
                  (doc->opt.for_prn) ? doc->view.line : cur->line, doc->total);
        if (doc->doctype == DB_LST) {
            if (Curlisting != NULL) {
                wnscrbar (doc->pwin, NO,
                            Curlisting->curcrt->fld_first->val.lst.numrec -
                                                Curlisting->firstnumrec,
                                                            Curlisting->total);
            } else {
                if (idlst.currecs < idlst.total)
                    wnscrbar (doc->pwin, NO, idlst.wasrecs + idlst.currecs / 2,
                                                         idlst.total);
            }
        }
        doc->curfld = cur->pfld;

        if (doc->doctype != DB_QRY &&
                    (dbgetidf (doc, cur->pfld))->valtype == SL_CDF ||
                                                        cur->pfld->opt.noedit)
            Db_edit = NO;

        if (doc->opt.cur_on) wntogcur (doc->opt.insert, NO);
        doc->opt.cur_on = (Db_edit || doc->opt.for_prn);

        if (Db_curidedt->fldwrk != NULL) (* Db_curidedt->fldwrk) ();

        if (ishide (cur->visicol, W, cur->beg, cur->len) ||
                                                utrange (cur->row, 0, H - 1)) {
            doc->opt.hide = YES;
            if (doc->opt.cur_on && !doc->opt.for_prn) doc->opt.cur_on = NO;
        } else {
            doc->opt.hide = NO;
            if (Db_edit) {
                if (!doc->opt.cur_on) doc->opt.cur_on = YES;
                if (cur->beg + doc->pos > cur->visicol + W - 1)
                    doc->pos = cur->visicol + W - 1 - cur->beg;
                else if (cur->beg + doc->pos < cur->visicol)
                    doc->pos = cur->visicol - cur->beg;
                wnsetcur (Wrow + cur->row, Wcol +cur->beg + doc->pos - cur->visicol);
            }
        }
        if (doc->opt.needout) dboutdoc (doc);
        if (doc->opt.for_prn)
            wnsetcur (Wrow + doc->view.row, Wcol + doc->view.pos);
        if (doc->opt.cur_on)
            wntogcur (doc->opt.insert, Db_edit || doc->opt.for_prn);
        doc->opt.scroll = doc->opt.needout = NO;
        doc->opt.changpos = YES;
        if (!cur->pfld->opt.wrap) doc->opt.move = YES;
/******************************************************************************/
        if (Db_curidedt->prewait != NULL) (* Db_curidedt->prewait) ();
        Db_Err = FLD_IS_VAL;
/******************************************************************************/

        if (!Db_MousMove) kbgetkey (dbedkhlp);
        if (Db_curidedt->aftergetkey != NULL) {
            switch (j = (* Db_curidedt->aftergetkey) ()) {
                case    0:      continue;
                case    1:      goto newdoc;
                case   -1:      ret = NO;
                case    2:      goto edtret;
                default  :      break;
            }
        }
        if (doc->opt.hide && Kb_sym != ShiftLeft && Kb_sym != ShiftRight &&
                    Kb_sym != ShiftUp && Kb_sym != ShiftDown) {
            doc->opt.hide = NO;
            doc->opt.needout = YES;
            * cur = * old;
            continue;
        }
        switch (vfydict ()) {
            case -1:        ret = YES; goto edtret;
            case  1:        continue;
            default :       break;
        }
        if (Db_curidedt->postwait != NULL) {
            switch (j = (* Db_curidedt->postwait) ()) {
                case    0:      continue;
                case    1:      goto newdoc;
                case   -1:      ret = NO;
                case    2:      goto edtret;
                default  :      break;
            }
        }
        dbed_nav ();
    }
edtret:
    if (top) wnremove (Db_phlp);
    free (doc->cur);
    free (doc->old);
    doc->cur = savcur;
    doc->old = savold;
    Db_main_edit = savmainedit;
    Db_edit      = savedit;
    Db_curidedt = savidedt;
    Cur_Edt = savdoc;
    msgetpos (&b, &l, &j);
    dbedmous (0, 0, l, b, 0, 0);
    mshide ();
    msshadow (&event, old_msfunc);
    return ret;
}

void    setnewfld (void) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;
    FLD         * pfold = doc->old->pfld, * pftmp;
    CRT         * curcrt;
    long          i;

    if (doc->opt.needfmt) {
        doc->opt.needfmt = NO;
        pftmp = nextcont (pfold, dbgetpapa (pfold));
        if (pfold->prnopt.full || pftmp == NULL || pftmp->prnopt.full
                                                        || pftmp->opt.par) {
            doc->curfld = cur->pfld;
            dboutcol (doc, pfold, NO);
            curcrt = doc->curfld->curcrt;
            if (curval != NULL && !curval->prnopt.full &&
                                  (!curcrt->opt.multy || curcrt->prev == NULL))
                curval->newfld = YES;
            dbfmtcol (NO);
            cur->pfld = doc->curfld;
            dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
            doc->opt.needout = YES;
            pftmp = firstvisi (doc->crtroot->fld_first);
            i = dbgetpno (doc, pftmp) + dbcalcln (doc, pftmp, doc->curfld);
            i -= cur->line;
            rowlinecorr ((int) i);
            cur->pfld->opt.change = NO;
        }
    }
    if (!doc->opt.scroll) getvisicol ();
    if (doc->cur->visicol != doc->old->visicol) doc->opt.needout = YES;
    if (Db_edit) {
        if (doc->opt.changpos) doc->pos = 0;
        val2pwrk ();
    }
    outfields ();
    cur2old ();
    return;
}

bool     dbsetfld (void) {
    bool retcode;

    if ((retcode = dbed_c2v ()) == YES) setnewfld ();
    return retcode;
}

void     outfields (void) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur, * old = doc->old;

    if (!doc->opt.needout) {
        dboutfld (doc, cur->pfld, cur->row, cur->beg, cur->len, NO, YES);
        dboutfld (doc, old->pfld, old->row, old->beg, old->len, NO, YES);
    }
    return;
}

void     jstmrk (void) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;

    pos2visicol ();
    if (cur->visicol != doc->old->visicol)  doc->opt.needout = YES;
    if (doc->opt.needout) { cur2old (); return; }
    dboutfld (doc, cur->pfld, cur->row, cur->beg, cur->len, NO, YES);
    return;
}

void     cur2old (void) {

    * Cur_Edt->old = * Cur_Edt->cur;
    return;
}

void     currestore (void) {

    utalarm ();
    * Cur_Edt->cur = * Cur_Edt->old;
    return;
}

void     rowlinecorr (register int i) {
    DOC      * doc = Cur_Edt;
    DOCEDT   * cur = doc->cur;
    register   H = doc->pwin->img.dim.h - 1;

    cur->row  += i;
    cur->line += i;
    if (utrange (cur->row, 0, H))
        { utbound (cur->row, 0, H); doc->opt.needout = YES; }
    return;
}

void     pos2visicol (void) {
    register    i, j;
    DOCEDT    * cur = Cur_Edt->cur;
    int         W = Cur_Edt->pwin->img.dim.w;
    int         W_4 = W / 4, pos = Cur_Edt->pos;
    int         beg = cur->beg, len = cur->len, visicol = cur->visicol;


    if (Cur_Edt->opt.for_prn) return;
    i = beg + len - visicol - W;
    j = visicol - beg;
    if (beg + pos > visicol + 3 * W_4 && i < W_4) {
        if (i >= 0) visicol = beg + len - W + 1;
    } else if (beg + pos < visicol + W_4 && j < W_4) {
        if (j >= 0) visicol = beg - 1;
    } else {
        i = beg + pos;
        if      (i < visicol)         visicol = i;
        else if (i > visicol + W - 1) visicol = i - (W - 1);
    }
    cur->visicol = max (visicol, 0);
    return;
}

void val2pwrk (void) {
    DOC       * doc = Cur_Edt;
    DOCEDT    * cur = doc->cur;
    byte far  * pwrk = doc->pwrk, * p;
    register    i;

    if (doc->opt.for_prn) return;
    p = pwrk;
    i = dbvaltoc (doc, cur->pfld, &p, cur->len);
    if (i > cur->len) i = cur->len;
    if (p != pwrk) utmovabs (pwrk, p, i, 0);
    utmovsc (&pwrk [i], ' ', cur->len - i + 4);
    pwrk [cur->len + 4] = EOS;
    return;
}

bool dbnewdoc (void) {
    DOC      * doc = Cur_Edt;
    DOCEDT   * cur = doc->cur;
    FLD      * pfld;
    int        H = doc->pwin->img.dim.h;
    int        W = doc->pwin->img.dim.w, b, l;

    Db_edit = Db_main_edit;
    wndsplay (Db_phlp, WN_NATIVE, WN_NATIVE);
    Db_Err = FLD_IS_VAL;
    doc->opt.needout = YES;
    pfld = cur->pfld = firstvisi (doc->crtroot->fld_first);
    cur->line = dbgetpno (doc, cur->pfld);
    Db_MousMove = NO;
    doc->opt.move = YES;
    while (cur->pfld->opt.protect) {
        cur->pfld = next (cur->pfld);
        if (cur->pfld == NULL) return NO;
    }
    cur->line += dbcalcln (doc, pfld, cur->pfld);
    doc->curfld = cur->pfld;
    dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
    cur->row = (int) cur->line;
    if (!doc->opt.for_prn && cur->row >= H) cur->row = H - 1;
    getvisicol ();
    if (Db_edit) {
        doc->pos = 0;
        val2pwrk ();
    }
    * doc->old = * cur;
    b = cur->beg; l = cur->len;
    dbcor_bl (cur->visicol, W, &b, &l);
    return YES;
}

static void     getvisicol (void) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;
    register      W = doc->pwin->img.dim.w;

    if (doc->opt.for_prn) return;
    cur->visicol = 0;
    if (cur->beg + cur->len + 1 > W) {
        if (cur->len + 1 <= W || (cur->pfld->prnopt.jst && !Db_edit))
            cur->visicol = cur->beg + cur->len + 1 - W;
        else  {
            cur->visicol = cur->beg - 1;
            cur->visicol = max (cur->visicol, 0);
        }
    }
    return;
}

static int vfydict (void) {
    DOC     * docfrom;
    IDFLD   * pidf;
    IDEDT   * pidedt;
    FLD     * pfldfrom, *pfldto;
    byte    * p;
    bool      retcode, dryuka = NO;
    register  len, lenfrom;
    int       valtype;

    if (Cur_Edt->doctype == DB_DICT) {
        if (Kb_sym != Enter) return 0;                 /* Клавиша не наша */
        dbed_c2v ();
        return -1;                              /* Завершать работу с CDF */
    }                                          /* и docedt возвращать YES */


    if (!Db_main_edit) return 0;

    docto = Cur_Edt;
    pfldto = docto->curfld;
    pidf = dbgetidf (docto, pfldto);
    if (pidf->initype != SL_DICT) return 0;
    if (Kb_sym == CtrlY && pidf->valtype == SL_CDF) {
        pfldto->prnopt.full = NO;
        val2pwrk ();
        docto->opt.needout = YES;
        return 1;
    }
    if (Kb_sym != F2) return 0;                /* Клавиша не наша */

    docfrom = pidf->fldlink.link.pdoc;

    dbdsplay (docfrom, WN_CENTER, WN_CENTER, NULL);
    pidedt = utalloc (IDEDT);
    pidedt->funkey   = cdfkey;
    pidedt->prewait  = Db_curidedt->prewait;
    pidedt->postwait = ed_dict;
    pidedt->row      = Db_curidedt->row;
    pidedt->col      = Db_curidedt->col;
    retcode = dbdocedt (docfrom, pidedt, NO, NO);
    free (pidedt);
    dbremove (docfrom);
    if (!retcode) return 1;

    pfldfrom = docfrom->curfld;
    Db_edit = YES;

    valtype = pidf->valtype;
    if (docto->doctype == DB_QRY) {
        if (pidf->initype == SL_DICT) dryuka = YES;
        valtype = SL_TEXT;
    }

    if (valtype == SL_TEXT) {
        p = docto->pwrk + docto->pos;
        lenfrom = pfldfrom->val.s.len;
        len = strlen ((char *) p);
        utmovmem ((char *) p + lenfrom + 2 * dryuka, (char *) p, len + 1);
        if (dryuka)  * p++ = '\\';
        utmovabs (p, pfldfrom->val.s.p, lenfrom, 0);
        if (dryuka)  * (p + lenfrom) = '\\';
        dbctoval (docto, pfldto, docto->pwrk, &pfldto->val);
        if (pfldto->opt.wrap) {
            dboutcol (docto, pfldto, NO);
            fmtcol ();
        }
    } else {
        pfldto->val = pfldfrom->next->val;
        pfldto->prnopt.full = YES;
        val2pwrk ();
    }
    docto->cur->pfld->opt.change  = NO;
    docto->opt.change             = YES;
    Db_edit = Db_main_edit;
    docto->opt.needout = YES;
    return 1;                              /* Обработали клавишу */
}

static int ed_dict (void) {
    bool    down = YES;

    switch (Kb_sym) {
        case F5:
            if (docto->doctype == DB_QRY) { utalarm (); break; }
            if (mnwarnin (noyes, 2, 4, " ",
                                "Вы уверены, что хотите",
                                          "изменить значение ?", " ") == 1)
                setnewcod ();
            break;
        case F6:
            down = NO;
        case F8:
            if (docto->doctype == DB_QRY) { utalarm (); break; }
            if (mnwarnin (noyes, 2, 4, " ",
                                "Вы уверены, что хотите",
                                          "ввести новое значение ?", " ") < 1)
                break;
            if (down) dbed_mdn (Cur_Edt->cur->pfld->curcrt);
            else      dbed_mup (Cur_Edt->cur->pfld->curcrt);
            setnewcod ();
            break;
        case F7:               dbprprn  (dbpreview);          return 1;
        case ShiftF7:          dbed_prf ();                   break;
        case ShiftF5:          dbed_2fl (Cur_Edt, YES);       break;
        case CtrlF4:           dbed_srt (YES);                break;
        case AltF4:            dbed_srt (NO);                 break;
        case Esc:                                          /* end of edit */
            if (!dbed_c2v ()) break;
            return -1;
        default :                                             return -2;
    }
    return 0;
}

static void setnewcod (void) {

    if (Cur_Edt->cur->pfld->next->val.i == 0) {
        Cur_Edt->cur->pfld->next->val.i = ++Cur_Edt->crtroot->fld_first->val.i;
        Cur_Edt->cur->pfld->next->prnopt.full    = YES;
        Cur_Edt->crtroot->fld_first->prnopt.full = YES;
    }
    Db_edit = YES;
    Cur_Edt->pos = 0;
    val2pwrk ();
    return;
}

