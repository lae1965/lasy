#include <sldbas.h>

static bool  lst_stop     (void);
static CRT * lst_getcrt   (void);
static void  lst_load     (bool down);
static void  lst_open     (void);
static void  lst_ini      (bool down);
static void  lst_close    (void);
static void  after_load   (void);
static bool  lst_makpage  (long num, bool down);
static bool  lst_makcrt   (bool down);

bool lsinilst (bool lstfile) {
    IDBASE    * pdb;
    DOC       * basedoc;
    int         handlst;

    Curlisting->lstfile = lstfile;
    if (lstfile && * Curlisting->filename == EOS)
        strcpy (Curlisting->filename, "lst.tmp");

    Curlisting->first     = Curlisting->firstnumrec = 1L;
    Curlisting->currecs   = 0;
    Curlisting->getlstcrt = lst_getcrt;
    Curlisting->open      = lst_open;
    Curlisting->ini       = lst_ini;
    Curlisting->load      = lst_load;
    Curlisting->stop      = lst_stop;
    Curlisting->close     = lst_close;
    Curlisting->makpage   = lst_makpage;
    Curlisting->makcrt    = lst_makcrt;
    Curlisting->H_page    = Cur_Lst->pwin->img.dim.h;

    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, Cur_Lst->opt._global);
    strcpy (Curlisting->crtnam, (char *) basedoc->cnam [0].nam);
    dbdstroy (basedoc);

    if (lstfile) {
        if ((handlst = utopen (Curlisting->filename, UT_R)) == -1)
            handlst = utcreate (Curlisting->filename);
        Curlisting->total = utfsize (handlst) / sizeof (long);
        utclose (handlst);
    } else {
        pdb = dbopendb (Cur_Lst->name_data, Cur_Lst->opt._global, UT_R);
        Curlisting->total = pdb->slhd.total;
        dbclosdb (pdb);
    }
    if (Curlisting->total == 0L) return NO;
    return YES;
}

static bool lst_makpage (long num, bool down) {
    bool    change = Cur_Lst->opt.change;

    if (Curlisting->total == 0L) return NO;
    if (down) {
        if (num > Curlisting->total) return NO;
    } else {
        if (num < 1L) return NO;
    }
    Curlisting->newnum = Curlisting->curnum = num;

    Curlisting->open (); /* Открывает (инициализирует) все basedoc, pdb, lsthandle */
    Curlisting->ini (down); /* Определяет первоначальные basedoc, pdb, numrec, lsthandle */

    dbclrdoc (Cur_Lst, YES);
    Curlisting->curcrtwrk = Curlisting->getlstcrt ();
    Curlisting->currecs = 0;
    while (1) {

        Curlisting->load (down);      /* Заполняет поля curcrt */
        after_load ();

        Curlisting->curnum += (down) ? 1 : -1;
        Curlisting->currecs++;
        if (down) {
            if (num + Curlisting->currecs - 1 == Curlisting->total) break;
        } else {
            if (num - Curlisting->currecs == 0L) break;
        }

        if (Curlisting->stop ()) break;
                  /* Определяет, не пора ли заканчивать заполнение listing'а */

        dbcrtmul (Cur_Lst, Curlisting->curcrtwrk, down, NO);
        if (down) Curlisting->curcrtwrk = Curlisting->curcrtwrk->next;
        else      Curlisting->curcrtwrk = Curlisting->curcrtwrk->prev;
    }

    if (down)   Curlisting->first = num;
    else        Curlisting->first = num - Curlisting->currecs + 1;

    Curlisting->close ();  /* Закрывает (освобождает) все basedoc, pdb, lsthandle */

    Cur_Lst->opt.change = change;
    return YES;
}

static bool   lst_makcrt (bool down) {
    CRT             * pcrt;
    long              num;
    bool              change = Cur_Lst->opt.change;

    pcrt = Curlisting->getlstcrt ();
    if (down) {
        if (Curlisting->first + Curlisting->currecs - 1 == Curlisting->total)
            return NO;
        while (pcrt->next != NULL) pcrt = pcrt->next;
        num = Curlisting->first + Curlisting->currecs;
    } else {
        if (Curlisting->first == 1L) return NO;
        num = Curlisting->first - 1;
    }


    Curlisting->newnum = Curlisting->curnum = num;
    if (Curlisting->open  != NULL) Curlisting->open ();
    if (Curlisting->ini   != NULL) Curlisting->ini (down);

    dbcrtmul (Cur_Edt, pcrt, down, NO);
    if (down) Curlisting->curcrtwrk = pcrt->next;
    else      Curlisting->curcrtwrk = pcrt->prev;
    if (Curlisting->load  != NULL) Curlisting->load (down);
    after_load ();

    Curlisting->currecs++;
    if (!down) Curlisting->first--;

    if (Curlisting->close != NULL) Curlisting->close ();

    Cur_Lst->opt.change = change;
    return YES;
}

static void after_load (void) {
    int     i;
    long    numrec;

    Curlisting->curcrtwrk->fld_first->val.lst.numrec = Curlisting->curbasedoc->numrec;
    Curlisting->curcrtwrk->fld_first->prnopt.full = YES;

    if (Curmarklst != NULL && Curmarklst->mark) {
        numrec = Curlisting->curbasedoc->numrec;
        for (i = 0; i < Curmarklst->nummark; i++) {
            if (Curmarklst->pmark [i] == numrec) {
                Curlisting->curcrtwrk->opt.mark = YES;
                break;
            }
        }
    }
    return;
}

static void lst_open (void) {

    Curlisting->curbasedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS,
                                                        Cur_Lst->opt._global);

    Curlisting->curpidb = dbopendb (Cur_Lst->name_data, Cur_Lst->opt._global, UT_R);
    if (Curlisting->lstfile)
        Curlisting->curhandle = utopen (Curlisting->filename, UT_R);
}

static void lst_ini (bool down) {    /* newnum ------> numrec */
    CRT     * maincrt;
    long      l, numrec, offset, lastnum;

    if (Curlisting->lstfile) return;

    if (Curlisting->currecs == 0) {
        numrec = Curlisting->firstnumrec;
        goto ret;
    }

    lastnum = Curlisting->first + Curlisting->currecs - 1;
    maincrt = Curlisting->getlstcrt ();
    if (Curlisting->newnum >= Curlisting->first && Curlisting->newnum <= lastnum) {
                                                    /* Curlisting->newnum в памяти */
        for (l = Curlisting->first; l < Curlisting->newnum; l++)
            maincrt = maincrt->next;
        numrec = maincrt->fld_first->val.lst.numrec;
    } else {                                    /* Curlisting->newnum не в памяти */
        if (Curlisting->newnum > lastnum) {
                          /* Curlisting->newnum больше последнего номера в памяти */
            l = lastnum;
            while (maincrt->next != NULL) maincrt = maincrt->next;
            numrec = maincrt->fld_first->val.lst.numrec;
            utlseek (Curlisting->curpidb->handh,
                               numrec * sizeof (long) + sizeof (SLHD), UT_BEG);
            while (l < Curlisting->newnum) {
                utread (Curlisting->curpidb->handh, &offset, sizeof (long));
                if (offset > -1L) l++;
                numrec++;
            }
        } else {            /* Curlisting->newnum меньше первого номера в памяти */
            numrec = maincrt->fld_first->val.lst.numrec;
            l = Curlisting->first;
            while (l > Curlisting->newnum) {
                utlseek (Curlisting->curpidb->handh,
                         (numrec - 2) * sizeof (long) + sizeof (SLHD), UT_BEG);
                utread (Curlisting->curpidb->handh, &offset, sizeof (long));
                if (offset > -1L) l--;
                numrec--;
            }
        }
    }
ret:
    Curlisting->curbasedoc->numrec = numrec - ((down) ? 1 : -1);

    return;
}

static void lst_load (bool down) {
    long    numrec;

    if (Curlisting->lstfile) {
        utlseek (Curlisting->curhandle, (Curlisting->curnum - 1) * sizeof (long), UT_BEG);
        utread (Curlisting->curhandle, &numrec, sizeof (long));
    } else  numrec = Curlisting->curbasedoc->numrec + ((down) ? 1 : -1);

    dblodrec (Curlisting->curbasedoc, Curlisting->curpidb, numrec, down);
    dbcrtocr (Cur_Lst, Curlisting->curbasedoc, Curlisting->curcrtwrk, Curlisting->curbasedoc->crtroot, NO);
}

static bool lst_stop (void) {    /* На один кортеж больше страницы */

    if (Cur_Lst->total > Curlisting->H_page) return YES;
    return NO;
}

static void lst_close (void) {

    if (Curlisting->lstfile) utclose (Curlisting->curhandle);
    dbclosdb (Curlisting->curpidb);
    dbdstroy (Curlisting->curbasedoc);
    Curlisting->curbasedoc = NULL;
}

static CRT * lst_getcrt (void) {
    CRT     * maincrt;
    char    * pname;

    pname = Curlisting->crtnam;
    maincrt = Cur_Lst->crtroot->lowcrt;

    while ((strcmp (pname, (char *) Cur_Lst->cnam [maincrt->idc].nam)) != 0)
        maincrt = maincrt->nextsis;

    return maincrt;
}

void lsmarkone (void) {
    int     i;
    long    l;

    if (!Curmarklst->mark) return;
    Curlisting->curcrt->opt.mark = !Curlisting->curcrt->opt.mark;
    l = Curlisting->curcrt->fld_first->val.lst.numrec;
    if (Curlisting->curcrt->opt.mark) {
        Curmarklst->pmark [Curmarklst->nummark] = l;
        Curmarklst->nummark++;
    } else {
        Curmarklst->nummark--;
        i = 0;
        while (Curmarklst->pmark [i] != l) i++;
        utmovabs (Curmarklst->pmark + i, Curmarklst->pmark + i + 1,
                                 (Curmarklst->nummark - i) * sizeof (long), 0);
    }
    Cur_Lst->opt.needout = YES;
    if (Curlisting->curcrt->next != NULL || Curlisting->makcrt (YES)) lsdown ();
    return;
}

void lsmarkall (void) {
    int     hand, i;
    long    numrec, l;

    if (!Curmarklst->mark) return;

    Curmarklst->nummark = (int) Curlisting->total;
    if (Curlisting->lstfile) {
        hand = utopen (Curlisting->filename, UT_R);
        utread (hand, Curmarklst->pmark, Curmarklst->nummark * sizeof (long));
    } else {
        hand = dbopen (Cur_Lst->name_data, DB_HEAD, Cur_Lst->opt._global, UT_R);
        l = Curlisting->firstnumrec - 1;
        utlseek (hand, l * sizeof (long) + sizeof (SLHD), UT_BEG);
        for (i = 0; i < Curmarklst->nummark; i++) {
            do {
                utread (hand, &numrec, sizeof (long));
                l++;
            } while (numrec == -1L);
            Curmarklst->pmark [i] = l;
        }
    }
    utclose (hand);
    lsremakpg ();
    return;
}

IDMARKLST * lsinimark (bool mark) {
    FLD         * pfld;

    Curmarklst = utalloc (IDMARKLST);

    if (mark) {
        Curmarklst->pmark = malloc ((int) Curlisting->total * sizeof (long));
        Curmarklst->mark = YES;
    }

    Curlisting->curcrt = Curlisting->getlstcrt ();

    pfld = firstvisi (Curlisting->curcrt->fld_first);

    Curlisting->firstrow = dbgetpno (Cur_Lst, pfld);
    Curlisting->duphei   = Cur_Lst->idc [Curlisting->curcrt->idc].idd.num - 1;
    Curlisting->row      = Curlisting->firstrow;
    Curlisting->visicol  = 0;

    Curlisting->hei = dbcalcln (Cur_Lst, firstvisi (Curlisting->curcrt->fld_first),
                                      lastvisi (Curlisting->curcrt->fld_end)) + 1;

    Curmarklst->markone = lsmarkone;
    Curmarklst->markall = lsmarkall;

    return Curmarklst;
}

void  lsdstroymark (void) {

    utfree (&Curmarklst->pmark);
    utfree (&Curmarklst);
    Curmarklst = NULL;
}

