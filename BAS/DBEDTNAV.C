#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <slldraw.h>

static bool     dbfindfld       (void);
static char     setldraw        (bool need);
static int      updowncode      (FLD * pfld);
static bool     ldupdown        (bool down);
static void     ldsright        (void);
static void     ldsleft         (void);
static bool     txtupdown       (bool down);
static int      otherfield      (bool down, bool newstr);
static void     vert_correct    (void);
static void     mistake         (void);
static void     hiderestore     (void);
static void     savedict        (void);


bool    dbed_nav (void) {

    switch (Kb_sym) {
        case ShiftRight:       dbedt_shor (YES);     break;
        case ShiftLeft:        dbedt_shor (NO);      break;
        case ShiftDown:        dbedt_sver (YES);     break;
        case ShiftUp:          dbedt_sver (NO);      break;
        case SpecK1:           dbed_sp1 ();          break;
        case Right:            dbed_rgt ();          break;
        case Left:             dbed_lft ();          break;
        case Tab:              dbed_tab (NO);        break;
        case ShiftTab:         dbed_tab (YES);       break;
        case Up:               dbed_ver (NO);        break;
        case Down:             dbed_ver (YES);       break;
        case PgUp:             dbed_pg  (NO);        break;
        case PgDn:             dbed_pg  (YES);       break;
        case CtrlHome:         dbed_ceh (NO);        break;
        case CtrlEnd:          dbed_ceh (YES);       break;
        default :              dbed_edt ();          break;
    }

    return YES;
}

void dbed_sp1 (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;

    if (!dbed_c2v ()) return;
    Db_MousX += cur->visicol;
    if (Db_MousY == cur->row &&
                    Db_MousX >= cur->beg && Db_MousX <= cur->beg + cur->len) {
        kbsetsym (Enter);
        return;
    }
    if (!dbfindfld ())   /* Переход невозможен */
        { currestore (); return; }
    setnewfld ();
    return;
}

static bool dbfindfld (void) {
    DOCEDT      * cur = Cur_Edt->cur;
    register      i = 0;
    bool          down = NO;


    if (Db_MousY > cur->row || Db_MousY == cur->row && Db_MousX > cur->beg) down = YES;
    while (1) {
        if (down) {
            if (Db_MousY <= cur->row + i) break;
            i += otherfield (YES, NO);
        } else {
            if (Db_MousY >= cur->row + i) break;
            i -= otherfield (NO, NO);
        }
        if (cur->pfld == NULL) return NO;
    }
    if (Db_MousY != cur->row + i) return NO;
    rowlinecorr (i);
    i = 0;
    while (1) {
        dbbeglen (Cur_Edt, cur->pfld, &cur->beg, &cur->len);
        if (!utrange (Db_MousX, cur->beg, cur->beg + cur->len)) break;
        i = otherfield (down, NO);
        if (i != 0 || cur->pfld == NULL) return NO;
    }
    return YES;
}

void dbed_tab (bool shift) {

    if (!dbed_c2v ()) return;
    if (!dbktab (shift))   /* Переход невозможен */
        { currestore (); return; }
    setnewfld ();
    return;
}

bool dbktab (bool shift) {
    DOCEDT      * cur = Cur_Edt->cur;
    register      i;

    i = otherfield (!shift, NO);
    if (cur->pfld == NULL) return NO;
    rowlinecorr ((shift) ? -i : i);
    dbbeglen (Cur_Edt, cur->pfld, &cur->beg, &cur->len);
    return YES;
}

void dbed_rgt (void) {
    DOC     * doc = Cur_Edt;
    FLD     * pfld = doc->cur->pfld;

    if (Db_edit) {
        if (doc->pos < doc->cur->len - 1)
            { doc->pos++; jstmrk (); return; }
        if (pfld->opt.wrap) {
            if ((pfld = nextcont (pfld, dbgetpapa (pfld))) != NULL) {
                doc->pos = 0; dbed_ver (YES); return;
            }
        }
    }
    dbed_tab (NO);
    return;
}

void dbed_lft (void) {
    DOC     * doc = Cur_Edt;
    FLD     * pfld = doc->cur->pfld;

    if (Db_edit) {
        if (doc->pos > 0) { doc->pos--; jstmrk (); return; }
        if (pfld->opt.wrap) {
            if ((pfld = prevcont (pfld, dbgetpapa (pfld))) != NULL) {
                dbed_ver (NO); doc->pos = doc->cur->len - 1; return;
            }
        }
    }
    dbed_tab (YES);
    return;
}

void dbed_ver (bool down) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    DOCEDT  * old = doc->old;
    register  i;

    if (!dbed_c2v ()) return;
    i = otherfield (down, YES);
    if (cur->pfld == NULL) {
        * Cur_Edt->cur = * Cur_Edt->old;
        dbedt_sver (down);
        return;
    }
    rowlinecorr ((down) ? i : -i);
    vert_correct ();
    if (cur->pfld->opt.wrap && old->pfld->opt.wrap &&
        cur->pfld->idn == old->pfld->idn ||  !doc->opt.nulpos) {
        if (!down || doc->opt.changpos) doc->pos += old->beg - cur->beg;
        utbound (doc->pos, 0, cur->len - 1);
        pos2visicol ();
        if (cur->visicol != old->visicol) doc->opt.needout = YES;
        doc->opt.changpos = NO;
    }
    setnewfld ();
    return;
}

void    dbed_pg (bool down) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pftmp, * pii = NULL;
    int       H = doc->pwin->img.dim.h;
    register  ii = 0, i = 0;

    if (!dbed_c2v ()) return;
    while (1) {
        pftmp = cur->pfld;
        if (down) cur->pfld = next (cur->pfld);
        else      cur->pfld = prev (cur->pfld);
        if (cur->pfld == NULL) {
            if (ii != 0) { cur->pfld = pii; i = ii; }
            break;
        }
        if (down) i += how_row (doc, pftmp, cur->pfld);
        else      i += how_row (doc, cur->pfld, pftmp);
        if (!cur->pfld->opt.protect && i != ii) {
            if (i > H - 1) {
                if (ii != 0) { cur->pfld = pii; i = ii; }
                break;
            }
            pii = cur->pfld;
            ii = i;
        }
    }
    if (cur->pfld == NULL) {
        * cur = * doc->old;
        kbsetsym (down ? CtrlPgDn : CtrlPgUp);
        return;
    }
    rowlinecorr ((down) ? i : -i);
    vert_correct ();
    setnewfld ();
    return;
}

void dbed_ceh (bool down) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pftmp;
    register  i = 0;

    if (!dbed_c2v ()) return;
    if (down) {
        if (cur->pfld->next == NULL) { utalarm (); return; }/*Nothing*/
        cur->pfld = lastvisi (doc->crtroot->fld_end);
        i = (int) (doc->lastline - cur->line);
    } else {
        if (cur->pfld->prev == NULL) { utalarm (); return; }/*Nothing*/
        cur->pfld = firstvisi (doc->crtroot->fld_first);
        i = (int) (cur->line - dbgetpno (doc, cur->pfld));
    }
    while (cur->pfld->opt.protect) {
        pftmp = cur->pfld;
        if (down) {
            cur->pfld = prev (cur->pfld);
            i -= how_row (doc, cur->pfld, pftmp);
        } else {
            cur->pfld = next (cur->pfld);
            i -= how_row (doc, pftmp, cur->pfld);
        }
    }
    if (cur->pfld == NULL) {
        * cur = * doc->old;
        kbsetsym (down ? CtrlPgDn : CtrlPgUp);
        return;
    }
    rowlinecorr ((down) ? i : -i);
    dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
    setnewfld ();
    return;
}

void  dbviewhor (bool right) {
    DOC * doc = Cur_Edt;

    if (right) {
        if (doc->view.pos == doc->pwin->img.dim.w - 1)
            { dbedt_shor (YES); return; }
        doc->view.pos++;
    } else {
        if (doc->view.pos == 0)   { dbedt_shor (NO);  return; }
        doc->view.pos--;
    }
    return;
}

void  dbviewver (bool down) {
    DOC     * doc = Cur_Edt;

    if (down) {
        if (doc->view.line == doc->total - 1) { utalarm (); return; }
        if (doc->view.row  == doc->pwin->img.dim.h - 1) dbedt_sver (YES);
        else doc->view.row++;
        doc->view.line++;
    } else {
        if (doc->view.line == 0L) { utalarm ();  return; }
        if (doc->view.row == 0) dbedt_sver (NO);
        else doc->view.row--;
        doc->view.line--;
    }
    return;
}

void dbed_sde (void) {
    DOC     * doc = Cur_Edt;

    if (!doc->cur->pfld->opt.wrap) { utalarm (); return; }
    if (doc->opt.single == 0) doc->opt.single = 2;
    else                      doc->opt.single--;
    return;
}

void dbed_mov (void) {
    DOC     * doc = Cur_Edt;

    if (doc->cur->pfld->opt.wrap) doc->opt.move = !doc->opt.move;
    else                          utalarm ();
    return;
}

bool    dbed_c2v (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pfld = doc->cur->pfld;
    int       tasknam;

    doc->opt.needfmt = NO;
    if (!pfld->opt.change) goto ret;
    if ((Db_Err = dbctoval (doc, pfld, doc->pwrk, &pfld->val)) > 1)
        { mistake (); return NO; }
    if (Db_curidedt->leavefld != NULL)
        if ((* Db_curidedt->leavefld) () > 1)
            { mistake (); return NO; }
    if (pfld->opt.wrap) doc->opt.needfmt = YES;
    else {
        if (doc->doctype == DB_DICT && Db_edit) savedict ();
        dbcalsum (doc, cur->pfld);
        tasknam = (dbgetidf (doc, cur->pfld))->fldlink.link.tasknam;
        if (tasknam != -1)  utspawn ((char *) doc->tnam [tasknam].nam, doc);
    }
    cur->pfld->opt.change = NO;

ret:
    Db_edit = Db_main_edit;
    return YES;
}

void    dbedt_shor (bool right) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    int        W = doc->pwin->img.dim.w;

    if (!doc->opt.move) {
        if (right) ldsright ();
        else       ldsleft  ();
        jstmrk ();
        return;
    }
    if (right) cur->visicol++;
    else {
        if (cur->visicol <= 0) { utalarm (); return; } /* Nothing */
        cur->visicol--;
    }
    doc->opt.scroll  = YES;             /* Не надо пересчитывать visicol */
    doc->opt.needout = YES;             /* Необходим перевывод экрана    */
    if (!ishide (cur->visicol, W, cur->beg, cur->len)) {
        cur2old (); return;   /* Сохранение новых координат    */
    }
    if (!dbed_c2v ()) return;
    if (otherfield (right, NO) != 0 || cur->pfld == NULL)
        { hiderestore (); return; }
    dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
    if (ishide (cur->visicol, W, cur->beg, cur->len)) hiderestore ();
    else                                              setnewfld   ();
    return;
}

void    dbedt_sver (bool down) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    int        H = doc->pwin->img.dim.h;
    register i;

    if (!doc->opt.move) {
        doc->opt.changpos = NO;
        if (!ldupdown (down)) { jstmrk (); return; }
        outfields ();   cur2old ();
        return;
    }
    if (down) {
        if (cur->line + (H - cur->row) >= doc->total) { utalarm ();  return; }
        cur->row--;
    } else {
        if (cur->line < cur->row + 1)                 { utalarm ();  return; }               /* Nothing */
        cur->row++;
    }
    doc->opt.scroll  = YES;             /* Не надо пересчитывать visicol */
    doc->opt.needout = YES;             /* Необходим перевывод экрана    */

    if (cur->row < H  && cur->row >= 0) { cur2old (); return; }
                                                 /* Текущее поле на экране */

    if ((down && cur->row >= H) || (!down && cur->row < 0)) return;
              /* Идем в направлении текущего поля, но оно еще не на экране */

    if (!dbed_c2v ()) return;

    i = otherfield (down, YES);      /* Пытаемся перейти на следующее поле */

    if (cur->pfld == NULL || (down && i > H - 1 - cur->row) ||
                                                    (!down && i > cur->row)) {
        hiderestore (); return;
    }    /* Тек.поле не на экране, а след-го нет или оно тоже не на экране */

    vert_correct ();                  /* Удалось перейти на следующее поле */
    rowlinecorr ((down) ? i : -i);
    setnewfld ();
    return;
}

static char     setldraw (bool need) {
    DOC     * doc = Cur_Edt;
    SYM_LD    right, left, up, down, center;
    FLD     * pfld = doc->cur->pfld;
    CRT     * papa;
    byte    * pwrk = doc->pwrk;
    register  pos = doc->pos, sym;

    center.x = 0;

    papa = dbgetpapa (pfld);

    left.x  = (pos == 0) ? 0  : ldgetcod (pwrk [pos - 1]);
    right.x = (pos >= doc->cur->len - 1) ? 0 : ldgetcod (pwrk [pos + 1]);

    up.x    = updowncode (prevcont (pfld, papa));
    down.x  = updowncode (nextcont (pfld, papa));

    center.b.lt_s = left.b.rt_s;
    center.b.lt_d = left.b.rt_d;
    center.b.rt_s = right.b.lt_s;
    center.b.rt_d = right.b.lt_d;
    center.b.up_s = up.b.dn_s;
    center.b.up_d = up.b.dn_d;
    center.b.dn_s = down.b.up_s;
    center.b.dn_d = down.b.up_d;

    sym = ldgetchr (center.x, doc->opt.single);
    if (!need || sym == 0) return (sym);
    pwrk [pos] = sym;
    doc->opt.change = pfld->opt.change = YES;
    return 0;
}

static int updowncode (FLD * pfld) {
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;
    register      i, ret;
    int           beg, pos = doc->pos;

    if (pfld == NULL) return 0;

    dbbeglen (doc, pfld, &beg, NULL);
    if ((i = beg - cur->beg) > pos || cur->beg + pos >= beg + pfld->val.s.len)
            ret = 0;
    else    ret = ldgetcod (pfld->val.s.p [pos - i]);
    return ret;
}

static bool ldupdown (bool down) {
    DOC        * doc = Cur_Edt;
    register     sym, pos = doc->pos;
    int          single = doc->opt.single;
    byte       * pwrk = doc->pwrk, cursym;
    bool         ret = NO;

    doc->cur->pfld->opt.par = YES;

    cursym = pwrk [pos];
    if (single == 2) {
        if (cursym ==  ' ') ret = txtupdown (down);
        pwrk [pos] = ' ';
    } else {
        sym = ldgetcod (cursym);
        if (sym != 0 && (sym == 80 && single || sym == 160 && !single ||
                         cursym == setldraw (NO)))
             ret = txtupdown (down);
        setldraw (YES);
    }
    return ret;
}

static void ldsright (void) {
    DOC         * doc = Cur_Edt;
    register      sym, pos = doc->pos;
    int           single = doc->opt.single;
    byte        * pwrk = doc->pwrk, cursym;

    doc->cur->pfld->opt.par = YES;

    cursym = pwrk [pos];

    if (single == 2) {
        if (cursym ==  ' ') {
            pos++;
            if (pos >= doc->cur->len) return;
        }
        pwrk [pos] = ' ';
    } else {
        sym = ldgetcod (cursym);
        if (sym != 0 && (sym == 5 && single || sym == 10 && !single ||
                 cursym == setldraw (NO))) pos++;
        if (pos >= doc->cur->len) return;
        doc->pos = pos;
        setldraw (YES);
    }
    doc->pos = pos;
    return;
}

static void ldsleft (void) {
    DOC         * doc = Cur_Edt;
    register      sym, pos = doc->pos;
    int           single = doc->opt.single;
    byte        * pwrk = doc->pwrk, cursym;

    doc->cur->pfld->opt.par = YES;

    cursym = pwrk [pos];

    if (single == 2) {
        if (cursym == ' ') pos--;
        if (pos < 0) return;
        pwrk [pos] = ' ';
    } else {
        sym = ldgetcod (cursym);
        if (sym != 0 && (sym == 5 && single || sym == 10 && !single ||
                     cursym == setldraw (NO))) pos--;
        if (pos < 0) return;
        doc->pos = pos;
        setldraw (YES);
    }
    doc->pos = pos;
    return;
}

static bool     txtupdown (bool down) {
    DOC       * doc = Cur_Edt;
    DOCEDT    * cur = doc->cur, * old = doc->old;
    FLD       * pfld = cur->pfld;
    CRT       * papa;
    register    i, pos = doc->pos;

    papa = dbgetpapa (pfld);

    if (down) {
        pfld = nextcont (pfld, papa);
        if (pfld == NULL || dbcalcln (doc, cur->pfld, pfld) != 1) return NO;
    } else {
        pfld = prevcont (pfld, papa);
        if (pfld == NULL || dbcalcln (doc, pfld, cur->pfld) != 1) return NO;
    }

    cur->pfld = pfld;
    dbbeglen (doc, pfld, &cur->beg, &cur->len);

    if ((i = cur->beg - old->beg) > pos ||
                                    old->beg + pos >= cur->beg + cur->len) {
        * cur = * old;
        return NO;
    }
    if (dbctoval (doc, old->pfld, doc->pwrk, &old->pfld->val) > 1) return NO;
    val2pwrk ();
    doc->pos -= i;
    rowlinecorr ((down) ? 1 : -1);
    pfld->opt.par = YES;
    pfld->prnopt = old->pfld->prnopt;
    return YES;
}

static int  otherfield (bool down, bool newstr) {
    DOC         * doc = Cur_Edt;
    FLD         * pfld = doc->cur->pfld, * ptmp = pfld;
    register      i = 0;

    while (1) {
        if (down)  pfld = next (pfld);
        else       pfld = prev (pfld);
        if (pfld == NULL) break;
        if (down)   i += how_row (doc, ptmp, pfld);
        else        i += how_row (doc, pfld, ptmp);
        ptmp = pfld;
        if (!pfld->opt.protect && (!newstr || i != 0)) break;
    }
    doc->cur->pfld = pfld;
    return i;
}

static void  vert_correct (void) {
    DOCEDT  * cur = Cur_Edt->cur;
    FLD     * f1 = cur->pfld, * f2, * pftmp;
    register  numbrick = 1;
    int       c1 = 0, c2 = -1, l1 = 0, l2, over1, over2;
    byte    * p;

    p = dbgetstr (Cur_Edt, f1);

    while (f1->instr != 1) f1 = f1->prev;
    while (1) {
        p += dbfindbr (p, &c1, &l1, numbrick);
        if (!f1->opt.protect) {
            if (c1 > cur->beg && c2 != -1) break;
            f2 = f1; c2 = c1; l2 = l1;
        }
        pftmp = f1;
        f1 = next (f1);
        if (f1 == NULL || f1->instr == 1)
            { cur->pfld = f2; cur->beg = c2; cur->len = l2; return; }
        numbrick = f1->instr - pftmp->instr;
    }
    over1 = cur->beg + cur->len -  c1;
    over2 = c2   +  l2  - cur->beg;
    if (over1 > over2) { cur->pfld = f1; cur->beg = c1; cur->len = l1; }
    else               { cur->pfld = f2; cur->beg = c2; cur->len = l2; }
    return;
}

static void     mistake (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;

    doc->opt.needout = NO;
    * cur = * doc->old;
    dboutfld (doc, cur->pfld, cur->row, cur->beg, cur->len, YES, YES);
    utalarm ();
    return;
}

static void     hiderestore (void) {
    DOCEDT      * cur = Cur_Edt->cur, * old = Cur_Edt->old;

    cur->pfld = old->pfld;
    cur->beg  = old->beg;
    cur->len  = old->len;
    return;
}

static  void  savedict (void) {
    DOC     * doc = Cur_Edt;
    int       hand;
    char      buf [13];

    utmovabs (buf, doc->name_data, 9, 0);
    utchext (buf, Db_ext [DB_VOC]);
    if ((hand = utcreate (buf)) != -1) {
        dbsavdoc (doc, doc->crtroot, hand);
        utclose (hand);
    }
}

