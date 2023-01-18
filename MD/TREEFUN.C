#include    "makedoc.h"
#include    <slkeyb.h>
#include    <slmous.h>
#include    <slsymb.def>
#include    <string.h>

static  void        outhlp      (void);
static bool far _Cdecl mstrhand (int ev, int bs, int xp, int yp, int xd, int yd);

static  char        ttl5[] = "поле  ";
static  char        ttl6[] = "кортеж  ";
static  char        ttl7[] = "Кортеж  ";
static  char        ttl8[] = "Выберите \"Да\", если необходима корректировка";
static  char        titl[80];
static  ITEM        yesno[] = {
        { " Да ",           "ДдLl",  NULL },
        { " Нет ",          "НнYy",  NULL },
        { " Продолжить ",   "ПпGg",  NULL }
};
static  int         y;

void    edttree (void) {
    bool      done = NO, hide = NO;
    int       key;
    int       event;
    bool      (far * _Cdecl old_msfunc)();
    WINDOW  * pwinhlp;
    int       Wrow = ptree->pwin->where_shown.row;
    MDNODE  * curpuser, * icepuser;

    pwinhlp = wnfunkey (outhlp, YES);
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS|MS_CENTER_PRESS|MS_MOVE;
    old_msfunc = msshadow (&event, mstrhand);

    while (!done) {
        y = Wrow + ptree->row;
        mssetpos (y, 8);
        kbflushb ();
        trouttr (ptree, hide);
        key = kbgetkey (outhlp);
        if (hide && key != ShiftLeft && key != ShiftRight &&
                    key != ShiftUp   && key != ShiftDown)
                { hide = NO; continue; }
        switch (key) {
            case    Down:       if (!trdown    (ptree))  utalarm ();     break;
            case    Up:         if (!trup      (ptree))  utalarm ();     break;
            case    ShiftLeft:  if (trsleft    (ptree))  hide = YES;
                                else                     utalarm ();
                                                                         break;
            case    ShiftRight: if (trsright   (ptree))  hide = YES;     break;
            case    ShiftUp:    if (trsup      (ptree))  hide = YES;
                                else                     utalarm ();
                                                                         break;
            case    ShiftDown:  if (trsdown    (ptree))  hide = YES;
                                else                     utalarm ();
                                                                         break;
            case    CtrlEnd:    if (!trcend    (ptree))  utalarm ();     break;
            case    CtrlHome:   if (!trchome   (ptree))  utalarm ();     break;
            case    CtrlPgUp:   if (!trcpgup   (ptree))  utalarm ();     break;
            case    CtrlPgDn:   if (!trcpgdn   (ptree))  utalarm ();     break;
            case    PgDn:       if (!trpgdn    (ptree))  utalarm ();     break;
            case    PgUp:       if (!trpgup    (ptree))  utalarm ();     break;
            case    Home:       if (!trhome    (ptree))  utalarm ();     break;
            case    End:        if (!trend     (ptree))  utalarm ();     break;
            case    F2:         vfytree        ();                       break;
            case    F3:         trtbleft       (ptree);                  break;
            case    F4:         trtbrght       (ptree);                  break;
            case    F5:
                if (trinshig (ptree, NO,  NULL, inipuser) == -1)    utalarm ();
                                                                         break;
            case    F6:
                if (trinshig (ptree, YES, NULL, inipuser) == -1)    utalarm ();
                                                                         break;
            case    F7:
                trinslow  (ptree, NO,  whereins (NO),  NULL, inipuser);  break;
            case    F8:
                trinslow  (ptree, YES, whereins (YES), NULL, inipuser);  break;
            case    F9:         trdelnod  (ptree, delnode);              break;
            case    F10:        trrename  (ptree);                       break;
            case    ShiftF10:
                if (ptree->cur->opt.fld)  {
                    if (ptree->cur == ptree->ice) killice (ptree);
                    else                          makeice (ptree);
                } else                            utalarm ();
                break;
            case    ShiftF7:    troutprn (ptree);                        break;
            case    GrayCtrlEnter:
            case    CtrlEnter:
                if (ptree->cur->opt.fld)  optbox (ptree->cur);
                else                      utalarm ();
                break;
            case    ShiftEnter:
            case    GrayShiftEnter:
            case    GrayEnter:
            case    Enter:
                if (ptree->ice == NULL || !ptree->cur->opt.fld)
                    { utalarm (); break; }
                curpuser = (MDNODE *) ptree->cur->puser;
                icepuser = (MDNODE *) ptree->ice->puser;
                curpuser->idf = icepuser->idf;
                utmovabs (curpuser->docname, icepuser->docname, MAX_NAM_LEN, 0);
                utmovabs (curpuser->fldup,   icepuser->fldup,   MAX_NAM_LEN, 0);
                utmovabs (curpuser->flddown, icepuser->flddown, MAX_NAM_LEN, 0);
                utmovabs (curpuser->taskname,icepuser->taskname, 8, 0);
                if (key == ShiftEnter || key == GrayShiftEnter)
                    utmovabs (ptree->cur->name,ptree->ice->name,MAX_NAM_LEN,0);
                killice (ptree);
                break;
            case    AltX:
                if (vfytree () > 0) done = YES;
                break;
            default :                                  utalarm ();    break;
        }
    }
    msshadow (&event, old_msfunc);
    wndstroy (pwinhlp);
    return;
}

int    vfytree (void) {                                /* whatdo */
    char        * name;                            /* < 0 - не выполняться */
    SLNODE      * p, * curcrt = ptree->root;       /*   0 - корректировка  */
    register      i, j;                            /* > 0 - выполняться    */
    int           reterr, whatdo = 100;

    for (i = 0; i < ptree->total; i++, curcrt = curcrt->next) {
        if (curcrt->opt.fld) continue;
        if ((reterr = vfytrcrt (curcrt)) == 0) {
            p = curcrt->next;
            name = curcrt->name;
            for (j = i + 1; j < ptree->total; j++, p = p->next) {
                if (p->opt.fld) continue;
                if (strcmp (name, p->name) == 0) { reterr = 4; break; }
            }
        }
        if (reterr == 0) continue;
        strcpy (titl, ttl7);    strcat (titl, curcrt->name);
        switch (reterr) {
            case    1:                      /* 1 - нет полей             */
                whatdo = mnwarnin (yesno, 2, 7, titl,
                 "не имеет ни одного поля.", " ",
                  "Кортеж должен содержать хотя бы одно поле!", " ", ttl8, " ");
                break;
            case    2:                      /* 2 - только 1 поле         */
                whatdo = mnwarnin (yesno, 2, 7, titl,
                    "содержит только одно поле.", " ",
                        "Необходимо добавить кортеж или поле!", " ", ttl8, " ");
                break;
            case    3:                      /* 3 - нет сыновей           */
                whatdo = mnwarnin (yesno, 2, 8, titl,
                  "не содержит ни одного поля и ни одного кортежа.", " ",
                    "Кортеж должен содержать хотя бы",
                      (maker.opt.exittyp == 0) ?
                        "поле + поле или кортеж + поле!" :
                          "одно поле или один кортеж", " ", ttl8, " ");
                break;
            case    4:                      /* Одинаковые имена кортежей */
                whatdo = mnwarnin (yesno, 2, 7,
                 "Вы создали несколько кортежей с именем", curcrt->name, " ",
                  "Каждый кортеж должен иметь уникальное имя!", " ", ttl8, " ");
                break;
            default  :  break;
        }
        break;
    }
    if (whatdo == 100) { valtree = YES; whatdo = 1; }
    else                 valtree =  NO;
    if (whatdo == 0) {
        p = ptree->cur = curcrt;
        if (i > ptree->pwin->img.dim.h / 2) {
            j = ptree->row = ptree->pwin->img.dim.h / 2;
            for (; j > 0; j--) p = p->prev;
            ptree->top = p;
        } else {
            ptree->top = ptree->root;
            ptree->row = i;
        }
    }
    return whatdo;
}

int    vfytrcrt (SLNODE * curcrt) {
    SLNODE    * p = curcrt->next;
    register    i;
    int         ret = 1;                        /* 1 - нет полей     */
                                                /* 2 - только 1 поле */
                                                /* 3 - нет сыновей   */

    if (maker.opt.exittyp > 0 && curcrt->nsons > 0) return 0;
    if (curcrt->nsons >= 2) {
        for (i = curcrt->nsons; i > 0; i--) {
            if (p->opt.fld) { ret = 0; break; }
            p = trnxtbrt (p);
        }
    }
    else if (curcrt->nsons == 1)
        { if (p->opt.fld) ret = 2; }
    else  ret = 3;
    return ret;
}

bool inipuser (SLNODE * pnode) {
    MDNODE   * puser;

    if ((pnode->puser = utalloc (MDNODE)) == NULL) return NO;
    puser = (MDNODE *) pnode->puser;
    if (pnode->opt.fld) {
        puser->idf.opt.visibl = 1;
        puser->idf.sym = ' ';
        puser->idf.upval.i =  32767;
        puser->idf.dnval.i = -32767;
        puser->idf.prnopt = dcb.fld_prnopt;
    }
    return YES;
}

bool delnode (SLNODE * pnode) {
    bool    fld = pnode->opt.fld;
    int     ret;

    if (!fld && pnode->nsons > 0) {
        mnwarnin (NULL, 0, 5, "Не могу удалить кортеж", pnode->name,
                 "Предварительно удалите подчинённые", "поля и кортежи", " ");
        return NO;
    }
    if (fld)  strcpy (titl, ttl5);
    else      strcpy (titl, ttl6);
    strcat (titl, pnode->name);  strcat (titl, " ?");
    ret = mnwarnin (yesno, 2,  3, "Подтвердите, удалить", titl, " ");
    if (ret == 0) { delpuser (pnode); return YES; }
    return NO;
}

bool delpuser (SLNODE * pnode) {
    MDNODE * puser = (MDNODE *) pnode->puser;

    if (puser->idf.valtype == SL_TEXT) free (puser->idf.inival.s.p);
    free (puser);
    return YES;
}

static ITEM itm[] = {
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

static char mes1[] = "Выберите  имя кортежа, в который";
static char mes2[] = "необходимо  вставить  новое поле";
static char mes3[] = "необходимо вставить новый кортеж";

int  whereins (bool fld) {
    WINDOW  * pswin;
    MENU    * pmsg, *psmnu;
    int       len0, len1, len2, pos, wid, ret;
    bool      ok;

    if (ptree->cur->opt.fld)        return 0;
    if (ptree->cur == ptree->root)  return 1;
    len0 = strlen (mes1);
    len1 = strlen (ptree->cur->father->name);
    len2 = strlen (ptree->cur->name);
    wid = max (len0 + 4, len1 + len2 + 6);
    itm[0].text = ptree->cur->father->name;
    itm[1].text = ptree->cur->name;

    pmsg = mncreate (6, wid, 0, 0, &vmnubrd, &Att_Mnu2);
    wnwrtbuf (pmsg->pwin, 1, (wid-len0)/2, len0, mes1, WN_NATIVE);
    if (fld) wnwrtbuf(pmsg->pwin,2,(wid-len0)/2, len0, mes2, WN_NATIVE);
    else     wnwrtbuf(pmsg->pwin,2,(wid-len0)/2, len0, mes3, WN_NATIVE);

    pos = (wid - len1 - len2 - 2) / 2;
    ok = mnitmadd (pmsg, 0, 4, pos, &itm[0]);
    ok = mnitmadd (pmsg, 1, 4, pos + len1 + 2, &itm[1]);

    if (!ok) { mndstroy (pmsg); return (-1); }
    pswin = Cur_Wind;
    psmnu = Cur_Menu;
    mndsplay (pmsg, WN_CENTER, WN_CENTER);
    ret = mnreadop (pmsg, 0);
    mndstroy (pmsg);
    Cur_Menu = psmnu;
    wnselect (pswin);
    return (ret);
}

/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
static  char fkey[] =
"1       2Verify 3LvTab\33 4LvTab\32 5CrIns\30 6FlIns\30 7CrIns\31 8FlIns\31 9Delate 10Rename";
static  char shiffkey[] =
"1       2       3       4       5       6       7Print  8       9       10Ice   ";
static  char ctrlfkey[] =
"1       2       3       4       5       6       7       8       9       10      ";
static  char altfkey[] =
"1       2       3       4       5       6       7       8       9       10      ";

static void outhlp (void) {
    char far    * pbuf, *pscreen;

    if      (Kb_sym & KB_ALTSTAT)   pbuf = (char far *) altfkey;
    else if (Kb_sym & KB_SHIFTSTAT) pbuf = (char far *) shiffkey;
    else if (Kb_sym & KB_CTRLSTAT)  pbuf = (char far *) ctrlfkey;
    else                            pbuf = (char far *) fkey;
    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS * 2, 0, Cmd[0]);
    return;
}

#pragma warn -par
static bool far _Cdecl mstrhand (int ev, int bs, int xp, int yp, int xd, int yd) {
    bool        ret = YES;

    kbflushb ();
    Kb_2_tail = Kb_2_head;

    switch (ev) {
        case MS_LEFT_PRESS:      kbsetsym (CtrlEnter);         break;
        case MS_RIGHT_PRESS:     kbsetsym (ShiftF10);          break;
        case MS_CENTER_PRESS:    kbsetsym (Enter);             break;
        default :
            if (yp < y)             kbsetsym (Up);
            else if (yp > y)        kbsetsym (Down);
            else ret = NO;
            break;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

