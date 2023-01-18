#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>

typedef struct {
    int     numrec;
    int     numcrt;
} IDMENULV;

static char * menukey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
" Enter                 Вход                  Esc               Выход            ",
"XXXXXXX                                     XXXXX                               "
};

static char * lastlstkey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
"Ins Отметка/Разотметка Gray+/- Отметка/Разотметка списка Enter Перенос Esc Выход",
"XXXX                  XXXXXXXXX                         XXXXXXX       XXXXX     "
};

static char * lastdockey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
" Enter                Выбор                  Esc               Выход            ",
"XXXXXXX                                     XXXXX                               "
};

static DOC      * Menudoc;
static CRT      * Curcrt;
static IDMENU   * Pidmenu;
static char       pfirstnam  [MAX_NAM_LEN + 1];
static char       lastlstnam [MAX_NAM_LEN + 1];
static char       lastdocnam [MAX_NAM_LEN + 1];
static long       lastnumrec;
static int        lastnumofrecs;
static bool       _global, vihod;

static void     menudoc         (void);
static bool     lastmenulst     (void);
static bool     lastmenudoc     (void);
static int      ed_menudoc      (void);
static int      ed_lastmenulst  (void);
static int      ed_lastmenudoc  (void);
static void     go2crt          (void);
static void     showlevel       (int level);
static void     savelastrec     (void);

void  dbmenulv (char * menunam, bool _glob, IDMENU * pidmenu) {
    char  * p;

    _global = _glob;
    Pidmenu = pidmenu;
    vihod   = NO;

    if (lastnumrec > 0L && lastmenulst ()) return;

    lastnumrec = 0L;

    p = dbfilnam (menunam, Db_ext [DB_DOC] + 1);
    if (p == NULL) return;
    utmovabs (pfirstnam, p, MAX_NAM_LEN, 0);

    menudoc ();

    return;
}

static void menudoc (void) {
    IDMENULV    * pidmenulv;
    IDEDT       * pidedt;
    CRT         * pcrt;
    bool          vnutr = YES;
    long          numrec = 1L;
    int           curlevel = 0, i;

    Menudoc = dbinidoc (pfirstnam, DB_VISI|DB_MAXWIN, DB_DOC, _global);
    pidmenulv = calloc (100, sizeof (IDMENULV));

    pidedt = utalloc (IDEDT);
    pidedt->funkey   = menukey;
    pidedt->prewait  = Db_curidedt->prewait;
    pidedt->postwait = ed_menudoc;
    pidedt->go2firstfld = go2crt;
    pidedt->row      = Db_curidedt->row;
    pidedt->col      = Db_curidedt->col;

    while (curlevel >= 0) {
        if (!dblodone (Menudoc, numrec, YES)) break;

        dbremove (Menudoc);
        dbdsplay (Menudoc, DB_CENTER, WN_CENTER, NULL);
        showlevel (curlevel);

        if (vnutr) Curcrt = NULL;
        else {
            pcrt = Menudoc->crtroot->lowcrt;
            for (i = pidmenulv [curlevel].numcrt; i > 1; i--) pcrt = pcrt->next;
            Curcrt = pcrt;
        }

        vnutr = dbdocedt (Menudoc, pidedt, NO, NO);

        if (vihod) break;

        if (vnutr) {          /*** Переход к новому уровню меню ***/
            numrec = Menudoc->curfld->next->val.l;
            pcrt = Menudoc->curfld->curcrt;
            i = 0;
            while (pcrt != NULL) {
                i++;
                pcrt = pcrt->prev;
            }
            pidmenulv [curlevel].numrec = (int) Menudoc->numrec;
            pidmenulv [curlevel].numcrt = i;
            curlevel++;
        } else {               /*** Возврат на уровень выше ***/
            curlevel--;
            numrec = pidmenulv [curlevel].numrec;
        }
    }
    free (pidedt);
    free (pidmenulv);
    dbdstroy (Menudoc);
    return;
}

static bool lastmenulst (void) {
    DOC     * savlst = Cur_Lst;
    IDEDT   * pidedt;
    FLD     * pfld;
    char    * pfilnam, * pname;
    bool      retcode;
    long      numrec;
    int       num_of_recs;

    if (lastnumrec > 0L) {
        numrec      = lastnumrec;
        num_of_recs = lastnumofrecs;
        pname       = lastlstnam;
    } else {
        pfld        = Menudoc->cur->pfld;
        numrec      = pfld->next->val.l;
        num_of_recs = pfld->next->next->val.i;
        pname       = (char *) pfld->next->next->next->next->val.s.p;
    }
    pfilnam = dbfilnam (pname, Db_ext [DB_LST] + 1);
    Cur_Lst = dbinidoc (pfilnam, DB_VISI|DB_MAXWIN, DB_LST, _global);

    Curlisting = utalloc (IDLIST);
    strcpy (Curlisting->filename, "lst.mnu");

    lsinilst (NO);
    Curlisting->total       = num_of_recs;
    Curlisting->firstnumrec = numrec;

    Curlisting->makpage (1L, YES);

    Curmarklst = lsinimark (Pidmenu->opt.mark);
    Curlisting->userfun = ed_lastmenulst;
    Cur_Lst->opt.for_lst = YES;

    dbdsplay (Cur_Lst, DB_CENTER, WN_CENTER, NULL);
    pidedt = utalloc (IDEDT);
    pidedt->funkey      = lastlstkey;
    pidedt->aftergetkey = lstscrnv;
    pidedt->updscreen   = updscreen;
    pidedt->prewait  = Db_curidedt->prewait;
    pidedt->row      = Db_curidedt->row;
    pidedt->col      = Db_curidedt->col;
    retcode = dbdocedt (Cur_Lst, pidedt, NO, NO);
    free (pidedt);

    lsdstroymark ();
    utremove (Curlisting->filename);
    utfree (&Curlisting);
    dbdstroy (Cur_Lst);
    Cur_Lst = savlst;

    return retcode;
}

static bool lastmenudoc (void) {
    DOC     * doc;
    IDEDT   * pidedt;
    char    * pfilnam, * pname;
    bool      retcode;
    int       opt = DB_VISI;

    if (Pidmenu->opt.maxwin) opt |= DB_MAXWIN;

    if (lastnumrec > 0L) pname = lastdocnam;
    else  pname = (char *) Menudoc->cur->pfld->next->next->next->val.s.p;

    pfilnam = dbfilnam (pname, Db_ext [DB_DOC] + 1);
    doc = dbinidoc (pfilnam, opt, DB_DOC, _global);

    dblodone (doc, Curlisting->curcrt->fld_first->val.lst.numrec, YES);

    dbdsplay (doc, WN_NATIVE, WN_NATIVE, NULL);

    pidedt = utalloc (IDEDT);
    pidedt->funkey   = lastdockey;
    pidedt->postwait = ed_lastmenudoc;

    pidedt->prewait  = Db_curidedt->prewait;
    pidedt->row      = Db_curidedt->row;
    pidedt->col      = Db_curidedt->col;
    retcode = dbdocedt (doc, pidedt, NO, NO);
    free (pidedt);

    dbdstroy (doc);
    return retcode;
}

static int ed_menudoc (void) {
    FLD   * pfld;

    switch (Kb_sym) {
        case Esc:
        case AltX:                                         /* end of edit */
            return -1;
        case GrayEnter:
        case Enter:
            pfld = Cur_Edt->cur->pfld->next;             /* numrec */
            if (!pfld->prnopt.full || pfld->val.l == 0L) return 0;
            if (pfld->next->next->prnopt.full) {
                if (lastmenulst ()) { vihod = YES; return 2; }
                break;
            }
            return 2;
        default :     return -2;
    }
    return 0;
}

static int ed_lastmenulst (void) {

    switch (Kb_sym) {
        case Esc:
        case AltX:    return -1;
        case GrayEnter:
        case Enter:
            if (Pidmenu->enterfn != NULL) {
                if (Pidmenu->enterfn ()) {
                    savelastrec ();
                    return 2;
                }
                return -1;
            }
        case F10:
            if (!Pidmenu->opt.go2doc) { utalarm (); break; }
            if (lastmenudoc ()) return 2;
            break;
        default :     return -2;
    }
    return 0;
}

static int ed_lastmenudoc (void) {

    switch (Kb_sym) {
        case Enter:
            if (Pidmenu->crt2crt == NULL) break;
            if (Pidmenu->crt2crt ()) {
                savelastrec ();
                return 2;
            }
            return -1;
        case Esc:
        case AltX:    return -1;
        default :     return -2;
    }
    return 0;
}

static void go2crt (void) {

    if (Curcrt == NULL) return;

    while (Cur_Edt->cur->pfld != Curcrt->fld_first) dbed_tab (NO);
    return;
}

static void showlevel (int level) {
    int           row, col;
    WINDOW      * pwin = Menudoc->pwin;
    int           W_1 = pwin->img.dim.w - 1;
    char far    * pbuf, * pscreen;

    if (pwin->bord.type == BORD_NO_BORDER) return;

    pbuf = malloc (W_1 + 100);

    col = pwin->where_shown.col;

    row = pwin->where_shown.row - 1;

    if (level > W_1) level = W_1;
    utmovsc (pbuf,     '', level);
    utmovsc (pbuf + level, '─', W_1 - level);

    pscreen = wnviptrl (row, col + 1);
    wnvicopy (&pbuf, &pscreen, 1, W_1, NUM_COLS * 2, 0, Cmd[0]);

    free (pbuf);
    return;
}

static void savelastrec (void) {

    if (lastnumrec > 0L) return;

    lastnumrec    = Curlisting->firstnumrec;
    lastnumofrecs = (int) Curlisting->total;
    strcpy (lastlstnam, (char *) Menudoc->cur->pfld->next->next->next->next->val.s.p);
    strcpy (lastdocnam, (char *) Menudoc->cur->pfld->next->next->next->val.s.p);

}

