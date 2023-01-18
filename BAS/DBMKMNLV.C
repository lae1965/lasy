#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>

typedef struct {
    int     numrec;
    int     numcrt;
} IDMENULV;

typedef struct {
    char        docnam [MAX_NAM_LEN + 1];
    char        basfilnam [9];
    long        numrec;
} BASLST;

static ITEM   noyes [] = {
                { " Нет ",        "НнYy",  NULL },
                { " Да ",         "ДдLl",  NULL }
};

static char * menukey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
" F6/F8    Множить /    F9    Удалить     Enter     Вход     Esc     Выход     ",
"XXXXXXX                 XXXX              XXXXXXX            XXXXX              "
};

static char * lastlstkey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
" F7   Печатать   F6/F8   Множить /   F9   Удалить   Enter   Вход   Esc  Выход ",
"XXXX            XXXXXXX               XXXX           XXXXXXX        XXXXX       "
};

static char * lastdockey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
" F6/F8        Множить /        F9         Удалить        Esc       Выход      ",
"XXXXXXX                         XXXX                      XXXXX                 "
};

static DOC      * Menudoc;
static CRT      * Curcrt;
static BASLST   * pbaslst;
static IDBASE   * Pdb_from, * Pdb_to;
static IDMKMENU * Pidmenu;
static SLHD       slhd;
static char       pfirstnam [MAX_NAM_LEN + 1];
static bool       _global, was_delrec, need_sqz;
static int        Num_of_bases, Numbase;

static void     menudoc         (void);
static void     lastmenulst     (void);
static void     lastmenudoc     (void);
static int      ed_menudoc      (void);
static int      ed_lastmenulst  (void);
static int      ed_lastmenudoc  (void);
static void     go2crt          (void);
static long     insitem         (int numitm);
static void     delitem         (int numitm);
static void     delmenu         (void);
static void     _delmenu        (DOC * doc, long numrec);
static void     _dellastmenu    (CRT * pcrt);
static void     showlevel       (int level);
static void     setmodify       (DOC * doc, FLD * pfdocnam);
static void     sqzmenu         (void);
static void     sqzmenu         (void);
static void     sqzone          (CRT * pcrt, int num);
static void     old2newbas      (int num);
static void     closePdb        (void);
static long     gettotal        (void);

void  dbmkmnlv (char * menunam, bool _glob, IDMKMENU * pidmenu) {
    char  * p;
    int     hand;

    hand = utopen ("slb.ini", UT_R);
    utread (hand, &Num_of_bases, sizeof (int));
    utclose (hand);

    pbaslst = calloc (Num_of_bases, sizeof (BASLST));

    p = dbfilnam (menunam, Db_ext [DB_DOC] + 1);
    if (p == NULL) return;
    utmovabs (pfirstnam, p, MAX_NAM_LEN, 0);
    _global = _glob;
    need_sqz = NO;
    Pidmenu = pidmenu;

    menudoc ();

    if (* pbaslst->docnam != EOS) sqzmenu ();
    else if (need_sqz) {
        hand = dbopen (pfirstnam, DB_HEAD, _global, UT_R);
        utread (hand, &slhd, sizeof (SLHD));
        utclose (hand);
        wndiswrk (slhd.total, 3, " ", "Производится сохранение данных", " ");
        dbsqzbas (pfirstnam, _global, YES, YES, NO);
        wnremwrk ();
    }
    utfree (&pbaslst);
    return;
}

static void menudoc (void) {
    IDMENULV    * pidmenulv;
    IDEDT       * pidedt;
    CRT         * pcrt;
    FLD         * pfld;
    bool          vnutr = YES, needsave = NO;
    long          numrec = 1L, savnumrec = 0L;
    int           curlevel = 0, i;

    Menudoc = dbinidoc (pfirstnam, DB_VISI|DB_MAXWIN, DB_DOC, _global);
    pidmenulv = calloc (100, sizeof (IDMENULV));

    dbdsplay (Menudoc, WN_CENTER, WN_CENTER, NULL);

    pidedt = utalloc (IDEDT);
    pidedt->funkey   = menukey;
    pidedt->prewait  = dbedstat;
    pidedt->postwait = ed_menudoc;
    pidedt->go2firstfld = go2crt;
    pidedt->row      =  0;
    pidedt->col      = 21;

    while (curlevel >= 0) {
        showlevel (curlevel);
        if (!dblodone (Menudoc, numrec, YES)) {
            dbclrdoc (Menudoc, NO);
            Menudoc->numrec = 0L;
            needsave = YES;
        }

        if (vnutr) Curcrt = NULL;
        else {
            pcrt = Menudoc->crtroot->lowcrt;
            for (i = pidmenulv [curlevel].numcrt; i > 1; i--) pcrt = pcrt->next;
            Curcrt = pcrt;
            pfld = pcrt->fld_first->next;
            if (was_delrec) {
                pfld->prnopt.full = NO;             /* Номер записи */
                needsave = YES;
                was_delrec = NO;
            }
            else if (!pfld->prnopt.full) {
                pfld->val.l = savnumrec;
                pfld->prnopt.full = YES;
                needsave = YES;
            }
        }

        vnutr = dbdocedt (Menudoc, pidedt, YES, (curlevel == 0));

        if (needsave || Menudoc->opt.change) {
            need_sqz = YES;
            if (!was_delrec) {
                dbsavrec (Menudoc, Menudoc->crtroot);
                needsave = NO;
            }
        }
        savnumrec = Menudoc->numrec;

        if (vnutr) {          /*** Переход к новому уровню меню ***/
            if (Menudoc->curfld->next->prnopt.full)
                numrec = Menudoc->curfld->next->val.l;
            else    numrec = 0L;

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
    strcpy (pfirstnam, Menudoc->name_data);
    free (pidedt);
    free (pidmenulv);
    dbdstroy (Menudoc);
    return;
}

static void lastmenulst (void) {
    DOC     * savlst = Cur_Lst;
    IDEDT   * pidedt;
    FLD     * pfld;
    char    * pfilnam;
    bool      retcode, change = NO;
    long      numrec, firstrec, offset, l;
    int       num_of_recs, hand, handh, i;

    pfld = Menudoc->cur->pfld;

    if (pfld->next->prnopt.full) numrec = pfld->next->val.l;

    if (pfld->next->next->prnopt.full) num_of_recs = pfld->next->next->val.i;
    else                               num_of_recs = 0;

    pfilnam = dbfilnam ((char *) pfld->next->next->next->next->val.s.p,
                                                         Db_ext [DB_LST] + 1);
    Cur_Lst = dbinidoc (pfilnam, DB_VISI|DB_MAXWIN, DB_LST, _global);

    Curlisting = utalloc (IDLIST);
    strcpy (Curlisting->filename, "lst.mnu");

    lsinilst (YES);

    Curlisting->total = num_of_recs;
    if (Curlisting->total == 0L) {
        numrec = insitem (1);
        change = YES;
    }

    hand = utopen (Curlisting->filename, UT_W);
    for (i = 0; i < Curlisting->total; i++, numrec++)
        utwrite (hand, &numrec, sizeof (long));
    utclose (hand);

    Curlisting->makpage (1L, YES);

    Curmarklst = lsinimark (NO);

    if (Pidmenu->ed_lastmenulst == NULL) Curlisting->userfun = ed_lastmenulst;
    else                        Curlisting->userfun = Pidmenu->ed_lastmenulst;

    Cur_Lst->opt.for_lst = YES;

    dbdsplay (Cur_Lst, WN_NATIVE, WN_NATIVE, NULL);
    pidedt = utalloc (IDEDT);
    if (Pidmenu->lastlstkey == NULL) pidedt->funkey = lastlstkey;
    else                             pidedt->funkey = Pidmenu->lastlstkey;
    pidedt->aftergetkey = lstscrnv;
    pidedt->updscreen   = updscreen;
    pidedt->prewait  = Db_curidedt->prewait;
    pidedt->row      =  0;
    pidedt->col      = 21;
    retcode = dbdocedt (Cur_Lst, pidedt, NO, NO);
    free (pidedt);

    if (retcode || change) {
        pfld = Menudoc->cur->pfld;
        hand  = utopen (Curlisting->filename, UT_R);
        handh = dbopen (Cur_Lst->name_data, DB_HEAD, _global, UT_RW);
        for (i = 0; i < Curlisting->total; i++) {
            utread (hand, &numrec, sizeof (long));
            utlseek (handh,
                        (numrec - 1) * sizeof (long) + sizeof (SLHD), UT_BEG);
            utread  (handh, &offset, sizeof (long));
            utlseek (handh, -((long) sizeof (long)), UT_CUR);
            l = -1L;
            utwrite (handh, &l, sizeof (long));
            l = utlseek (handh, 0L, UT_END);
            utwrite (handh, &offset, sizeof (long));
            if (i == 0) firstrec = (l - sizeof (SLHD)) / sizeof (long) + 1;
        }
        utclose (handh);
        utclose (hand);

        pfld->next->next->val.i = (int) Curlisting->total;
        pfld->next->next->prnopt.full = (Curlisting->total > 0L);

        if (Curlisting->total == 0) {
            pfld = pfld->next;                  /* Номер записи */
            pfld->prnopt.full = NO;
            pfld = pfld->next;                  /* Кол-во записей */
            pfld->prnopt.full = NO;
            pfld = pfld->next;                  /* Имя документа */
            free (pfld->val.s.p);
            pfld->val.d = 0.0;
            pfld->prnopt.full = NO;
            pfld = pfld->next;                  /* Имя листинга */
            free (pfld->val.s.p);
            pfld->val.d = 0.0;
            pfld->prnopt.full = NO;
        } else {
            pfld->next->val.l = firstrec;
            pfld->next->prnopt.full = YES;
        }
        Menudoc->opt.change = YES;
    }

    utremove (Curlisting->filename);
    utfree (&Curmarklst);
    utfree (&Curlisting);
    dbdstroy (Cur_Lst);
    Cur_Lst = savlst;

    return;
}

static void lastmenudoc (void) {
    DOC     * doc;
    IDEDT   * pidedt;
    char    * pfilnam;
    bool      retcode;
    int       opt = DB_VISI;

    if (Pidmenu->maxwin) opt |= DB_MAXWIN;
    pfilnam = dbfilnam ((char *) Menudoc->cur->pfld->next->next->next->val.s.p,
                                                           Db_ext [DB_DOC] + 1);
    doc = dbinidoc (pfilnam, opt, DB_DOC, _global);

    dblodone (doc, Curlisting->curcrt->fld_first->val.lst.numrec, YES);

    dbdsplay (doc, WN_NATIVE, WN_NATIVE, NULL);

    pidedt = utalloc (IDEDT);
    pidedt->prewait  = dbedstat;
    if (Pidmenu->lastdockey == NULL) pidedt->funkey = lastdockey;
    else                             pidedt->funkey = Pidmenu->lastdockey;
    if (Pidmenu->ed_lastmenudoc == NULL) pidedt->postwait = ed_lastmenudoc;
    else                        pidedt->postwait = Pidmenu->ed_lastmenudoc;

    pidedt->row      =  0;
    pidedt->col      = 21;
    retcode = dbdocedt (doc, pidedt, YES, NO);
    free (pidedt);

    if (retcode) {
        dbsavrec (doc, doc->crtroot);
        lsremakpg ();
    }
    dbdstroy (doc);
    return;
}

static int ed_menudoc (void) {
    CRT   * pcrt;
    FLD   * pfld;
    bool    is_lastlevel;

    switch (Kb_sym) {
        case Esc:
        case AltX:                                         /* end of edit */
            dbsetfld ();
            return -1;
        case GrayEnter:
        case Enter:
            dbsetfld ();
            pfld = Cur_Edt->cur->pfld->next;             /* numrec */
            if (pfld->prnopt.full && pfld->val.l > 0L)
                is_lastlevel = pfld->next->next->prnopt.full;
            else {
                if (Pidmenu->enterfn == NULL) return 0;
                switch (Pidmenu->enterfn ()) {
                    case 0:                /* Последний уровень меню */
                        is_lastlevel = YES;
                        break;
                    case 1:                /* Не последний уровень меню */
                        is_lastlevel = NO;
                        break;
                    default :   return 0;  /* Отказ, продолжить в тек.уровне */
                }
            }

            if (is_lastlevel) { lastmenulst (); break; }
            return 2;
        case F6:      dbed_mup (Cur_Edt->cur->pfld->curcrt);  break;
        case F8:      dbed_mdn (Cur_Edt->cur->pfld->curcrt);  break;
        case F9:
            pfld = Cur_Edt->cur->pfld->next;
            if (mnwarnin (noyes, 2, 4, " ", "Вы уверены,",
                                      "что хотите удалять?", " ") != 1) break;
            if (pfld->prnopt.full && pfld->val.l > 0L) {
                if (mnwarnin (noyes, 2, 4, " ",
                        "Данный пункт имеет подпункты!",
                                            "Будем удалять?", " ") != 1) break;
                delmenu ();
            }
            pcrt = Cur_Edt->cur->pfld->curcrt;
            if (pcrt->next == NULL && pcrt->prev == NULL) {
                dbdelone (Cur_Edt);
                was_delrec = YES;
                return -1;
            }
            dbed_del (Cur_Edt->cur->pfld->curcrt, NO);
            break;
        default :     return -2;
    }
    return 0;
}

static int ed_lastmenulst (void) {
    CRT     * pcrt, * pcrt1;
    long      numrec;
    bool      down = YES;
    int       i;

    switch (Kb_sym) {
        case Esc:
        case AltX:                                         /* end of edit */
            if (!Cur_Edt->opt.change) return -1;
            return 2;
        case GrayEnter:
        case Enter:
            lastmenudoc ();
            break;
        case F6:
            down = NO;
        case F8:
            pcrt = Curlisting->curcrt;
            dbcrtmul (Cur_Edt, pcrt, down, NO);
            if (down) pcrt = pcrt->next;
            else {
                pcrt = pcrt->prev;
                Curlisting->curcrt = pcrt;
            }
            pcrt1 = pcrt;
            i = 0;
            while (pcrt != NULL) {
                pcrt = pcrt->prev;
                i++;
            }
            numrec = insitem ((int) (Curlisting->first + i - 1));
            pcrt1->fld_first->val.lst.numrec = numrec;
            pcrt1->fld_first->prnopt.full = YES;
            Curlisting->currecs++;

            if (down) lsdown ();
            else      crt2screen ();
            break;
        case F9:
            if (mnwarnin (noyes, 2, 4, " ", "Вы уверены,",
                                       "что хотите удалять?", " ") != 1) break;
            pcrt = Curlisting->curcrt;
            i = 0;
            while (pcrt != NULL) {
                pcrt = pcrt->prev;
                i++;
            }
            delitem ((int) (Curlisting->first + i - 1));
            Curlisting->currecs--;

            pcrt = Curlisting->curcrt;
            if (pcrt->next != NULL || Curlisting->makcrt (YES))
                Curlisting->curcrt = pcrt->next;
            else if (pcrt->prev != NULL || Curlisting->makcrt (NO))
                lsup ();
            else    return 2;

            dbcrtdel (Cur_Lst, pcrt);

            crt2screen ();
            break;
        case F7:      lsprnlst ();      return 1;
        default :     return -2;
    }
    return 0;
}

static int ed_lastmenudoc (void) {

    switch (Kb_sym) {
        case Esc:
        case AltX:                                         /* end of edit */
            if (!dbsetfld ()) break;
            if (!Cur_Edt->opt.change) return -1;
            setmodify (Cur_Edt, Menudoc->cur->pfld->next->next->next);
            return 2;
        case F6:      dbed_mup (Cur_Edt->cur->pfld->curcrt);      break;
        case F8:      dbed_mdn (Cur_Edt->cur->pfld->curcrt);      break;
        case F9:      dbed_del (Cur_Edt->cur->pfld->curcrt, YES); break;
        default :     return -2;
    }
    return 0;
}

static void go2crt (void) {

    if (Curcrt == NULL) return;

    while (Cur_Edt->cur->pfld != Curcrt->fld_first) dbed_tab (NO);
    return;
}

static long insitem (int numitm) {
    DOC     * basedoc;
    long    * pnum, numrec, offset;
    int       hand, remain;

    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, _global);
    dbsavrec (basedoc, basedoc->crtroot);
    numrec = basedoc->numrec;
    dbdstroy (basedoc);

    hand = utopen (Curlisting->filename, UT_RW);
    remain = (int) Curlisting->total - numitm + 1;
    pnum = malloc ((remain + 1) * sizeof (long));
    * pnum = numrec;
    offset = (numitm - 1) * sizeof (long);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, pnum + 1, remain * sizeof (long));
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, pnum, (remain + 1) * sizeof (long));
    free (pnum);
    utclose (hand);

    Cur_Lst->opt.change = YES;
    Curlisting->total++;
    setmodify (Cur_Lst, Menudoc->cur->pfld->next->next->next);

    return numrec;
}

static void delitem (int numitm) {
    DOC     * basedoc;
    long    * pnum, numrec, offset;
    int       hand, remain;

    hand = utopen (Curlisting->filename, UT_RW);
    remain = (int) Curlisting->total - numitm + 1;
    pnum = malloc (remain * sizeof (long));
    offset = (numitm - 1) * sizeof (long);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, pnum, remain * sizeof (long));
    numrec = * pnum;
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, pnum + 1, (remain - 1) * sizeof (long));
    free (pnum);
    Curlisting->total--;
    utchsize (hand, Curlisting->total * sizeof (long));
    utclose (hand);

    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, _global);
    basedoc->numrec = numrec;
    dbdelone (basedoc);
    dbdstroy (basedoc);

    Cur_Lst->opt.change = YES;
    setmodify (Cur_Lst, Menudoc->cur->pfld->next->next->next);
    return;
}

static void delmenu (void) {
    DOC         * doc;

    if (Menudoc->curfld->next->next->next->prnopt.full)
        _dellastmenu (Menudoc->curfld->curcrt);
    else {
        doc = dbinidoc (pfirstnam, 0, DB_DOC, _global);
        _delmenu (doc, Menudoc->curfld->next->val.l);
        dbdstroy (doc);
    }

    return;
}

static void _delmenu (DOC * doc, long numrec) {
    CRT         * pcrt;
    int           numcrt = 1, i;

    dblodone (doc, numrec, YES);
    pcrt = doc->crtroot->lowcrt;

    while (pcrt != NULL) {
        if (pcrt->fld_first->next->prnopt.full) {
            if (pcrt->fld_first->next->next->next->prnopt.full)
                _dellastmenu (pcrt);
            else    _delmenu (doc, pcrt->fld_first->next->val.l);
            dblodone (doc, numrec, YES);
            pcrt = doc->crtroot->lowcrt;
            for (i = numcrt; i > 1; i--) pcrt = pcrt->next;
        }
        pcrt = pcrt->next;
        numcrt++;
    }

    dbdelone (doc);
    return;
}

static void _dellastmenu (CRT * pcrt) {
    DOC     * doc;
    IDBASE  * pdb;
    char    * pfilnam;
    FLD     * pfdocnam;
    long      numrec;
    int       num_of_recs, i;

    numrec      = pcrt->fld_first->next->val.l;
    num_of_recs = pcrt->fld_first->next->next->val.i;

    pfdocnam = pcrt->fld_first->next->next->next;
    pfilnam = dbfilnam ((char *) pfdocnam->val.s.p, Db_ext [DB_DOC] + 1);
    doc = dbinidoc (pfilnam, 0, DB_DOC, _global);
    pdb = dbopendb (doc->name_data, _global, UT_RW);

    for (i = 0; i < num_of_recs; i++) dbdelrec (pdb, numrec + i);

    dbclosdb (pdb);
    setmodify (doc, pfdocnam);
    dbdstroy (doc);
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

static void setmodify (DOC * doc, FLD * pfdocnam) {
    char    * pdocname = (char *) pfdocnam->val.s.p;
    int       i = 0;

    while (1) {
        if (* pbaslst [i].docnam == EOS) {
            strcpy (pbaslst [i].docnam, pdocname);
            strcpy (pbaslst [i].basfilnam, doc->name_data);
            break;
        }
        if (strcmp (pbaslst [i].docnam, pdocname) == 0) break;
        i++;
    }
    return;
}

static void sqzmenu (void) {
    IDBASE      * pdbfrom, * pdbto;
    CRT         * pcrt;
    FLD         * pfld;
    char        * pdocnam;
    long          total, l, numrec;
    int           i, j;

    Numbase = -1;
    Menudoc = dbinidoc (pfirstnam, 0, DB_BAS, _global);
    pdbfrom = dbopendb (pfirstnam, _global, UT_R);
    pdbto   = dbcreatbas (pfirstnam, _global, DB_BAS, "00000000", NO);
    total   = pdbfrom->slhd.total;

    if (total > 0) wndiswrk (total + gettotal (), 3,
                                   " ", "Производится сохранение данных", " ");

    while (total > 0) {
        numrec = Menudoc->numrec + 1;
        dblodrec (Menudoc, pdbfrom, numrec, YES);
        wnupdwrk (1L);
        pcrt = Menudoc->crtroot->lowcrt;
        while (pcrt != NULL) {
            pfld = pcrt->fld_first->next->next->next;
            if (pfld->prnopt.full) {
                i = 0;
                pdocnam = (char *) pfld->val.s.p;
                while (* pbaslst [i].docnam != EOS) {
                    if (strcmp (pbaslst [i].docnam, pdocnam) == 0) {
                        sqzone (pcrt, i);
                        break;
                    }
                    i++;
                }
            }
            pcrt = pcrt->next;
        }
        l = -1L;
        utlseek (pdbto->handh, 0L, UT_END);
        for (j = (int) (Menudoc->numrec - numrec); j > 0; j--)
            utwrite (pdbto->handh, &l, sizeof (long));

        Menudoc->numrec = 0L;
        dbsavtmp (Menudoc, Menudoc->crtroot, pdbto);
        total--;
    }

    total   = pdbfrom->slhd.total;

    closePdb ();
    dbclosdb (pdbto);
    dbclosdb (pdbfrom);

    if (total > 0) {
        wnremwrk ();
        i = 0;
        while (i < Num_of_bases && * pbaslst [i].docnam != EOS)
            { old2newbas (i); i++; }


        dbdelbas (pfirstnam, _global, NO);
        dbrenambas ("00000000", NO, pfirstnam, _global, NO);
    }
    dbdstroy (Menudoc);
    return;
}

static void     sqzone (CRT * pcrt, int num) {
    char          basetonam [9];
    long          numrecfrom, numrecto, offset;
    long        * pnumrec;
    int           num_of_recs, i;

    numrecto    = pbaslst [num].numrec;
    numrecfrom  = pcrt->fld_first->next->val.l;
    num_of_recs = pcrt->fld_first->next->next->val.i;
    pcrt->fld_first->next->val.l = numrecto + 1;

    utl2crdx ((long) num + 1, basetonam, -8, 10);

    if (num != Numbase) {
        closePdb ();
        Pdb_from    = dbopendb (pbaslst [num].basfilnam, _global, UT_R);

        if (numrecto == 0L) dbcreatmp (basetonam);

        Pdb_to = dbopendb (basetonam, NO, UT_W);

        if (numrecto == 0L) {
            utlseek   (Pdb_from->handb, 0L, UT_BEG);
            utlseek   (Pdb_to->handb,   0L, UT_BEG);
            uthndcopy (Pdb_to->handb, Pdb_from->handb, Pdb_from->slhd.treelen);
            utlseek   (Pdb_from->handh, 0L, UT_BEG);
            utlseek   (Pdb_to->handh,   0L, UT_BEG);
            uthndcopy (Pdb_to->handh, Pdb_from->handh, sizeof (SLHD));
        }
        Numbase = num;
    }

    pnumrec = calloc (num_of_recs, sizeof (long));
    utlseek (Pdb_from->handh,
                    (numrecfrom - 1) * sizeof (long) + sizeof (SLHD), UT_BEG);
    utread  (Pdb_from->handh, pnumrec, num_of_recs * sizeof (long));
    for (i = 0; i < num_of_recs; i++) {
        offset = utlseek (Pdb_to->handb, 0L, UT_END);
        utlseek (Pdb_from->handb, pnumrec [i], UT_BEG);
        pnumrec [i] = offset;
        utread  (Pdb_from->handb, &offset, sizeof (long)); /* Зачитали длину */
        utlseek (Pdb_from->handb, -((long) sizeof (long)), UT_CUR);
        wnupdwrk (1L);
        uthndcopy (Pdb_to->handb, Pdb_from->handb, offset);
    }
    utlseek (Pdb_to->handh, 0L, UT_END);
    utwrite (Pdb_to->handh, pnumrec, num_of_recs * sizeof (long));
    free (pnumrec);

    pbaslst [num].numrec += num_of_recs;

    return;
}

static void old2newbas (int num) {
    char          basetonam [9];

    utl2crdx ((long) num + 1, basetonam, -8, 10);
    dbdelbas (pbaslst [num].basfilnam, _global, NO);
    dbrenambas (basetonam, NO, pbaslst [num].basfilnam, _global, NO);
}

static void closePdb (void) {

    if (Numbase != -1) {
        dbclosdb (Pdb_to);
        dbclosdb (Pdb_from);
    }
}

static long     gettotal (void) {
    int     i, hand;
    long    total = 0L;

    for (i = 0; i < Num_of_bases; i++) {
        if (* pbaslst [i].docnam == EOS) break;
        hand = dbopen (pbaslst [i].basfilnam, DB_HEAD, _global, UT_R);
        utread (hand, &slhd, sizeof (SLHD));
        utclose (hand);
        total += slhd.total;
    }
    return total;
}

