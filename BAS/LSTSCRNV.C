#include <sldbas.h>
#include <slkeyb.h>
#include <slsymb.def>

static void deload (CRT * pcrtfirst, bool down);
static void getcurcrt (void);

int    lstscrnv (void) {
    int     i;

    Cur_Edt->opt.scroll = YES;
    Cur_Edt->opt.needout = YES;

    if (Curlisting->userfun != NULL) {
        i = (* Curlisting->userfun) ();
        if (i != -2) return i;
    }

    switch (Kb_sym) {
        case Ins:
            if (Curmarklst == NULL) break;
            Curmarklst->markone ();
            break;
        case GrayPlus:
            if (Curmarklst == NULL) break;
            Curmarklst->markall ();
            break;
        case GrayMinus:
            if (Curmarklst == NULL) break;
            Curmarklst->nummark = 0;
            lsremakpg ();
            break;
        case Tab:
        case ShiftDown:
        case Down:                  lsdown ();      break;
        case ShiftTab:
        case ShiftUp:
        case Up:                    lsup ();        break;
        case PgUp:                  lspgup ();      break;
        case PgDn:                  lspgdn ();      break;
        case Home:                  lshome ();      break;
        case End:                   lsend ();       break;
        case ShiftRight:
        case Right:
            Curlisting->visicol++;
            Cur_Edt->cur->visicol = Curlisting->visicol;
            break;
        case ShiftLeft:
        case Left:
            if (Curlisting->visicol > 0) {
                Curlisting->visicol--;
                Cur_Edt->cur->visicol = Curlisting->visicol;
            }
            break;

        case SpecK1:                lsspec1 ();     break;

        case AltX:
        case Esc:                   return -1;
        default :                   break;
    }

    return 0;
}

void lsspec1 (void) {
    CRT     * pcrt;
    int       row = Curlisting->firstrow, hei;

    pcrt = Curlisting->getlstcrt ();

    while (1) {
        hei = dbcalcln (Cur_Edt, firstvisi (pcrt->fld_first),
                                                 lastvisi (pcrt->fld_end)) + 1;
        if (row <= Db_MousY && row + hei - 1 >= Db_MousY) break;
        row += hei + Curlisting->duphei;
        pcrt = pcrt->next;
        if (pcrt == NULL) { utalarm (); return; }
    }

    if (pcrt == Curlisting->curcrt) kbsetsym (Enter);
    else {
        Curlisting->curcrt = pcrt;
        Curlisting->row    = row;
        crt2screen ();
    }
}

void lsdown (void) {
    int       i;

    if (Curlisting->curcrt->next == NULL)
        if (!Curlisting->makcrt (YES)) { utalarm (); return; }

    i = dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->fld_first),
                               firstvisi (Curlisting->curcrt->next->fld_first));
    Curlisting->curcrt = Curlisting->curcrt->next;
    Curlisting->row += i;
    crt2screen ();
    return;
}

void lsup (void) {
    int       i;

    if (Curlisting->curcrt->prev == NULL)
        if (!Curlisting->makcrt (NO)) { utalarm (); return; }

    i = dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->prev->fld_first),
                                     firstvisi (Curlisting->curcrt->fld_first));
    Curlisting->curcrt = Curlisting->curcrt->prev;
    Curlisting->row -= i;
    crt2screen ();
    return;
}

void lspgdn (void) {
    CRT   * pcrt = Curlisting->curcrt;
    long    lastnum;
    int     i;

    while (pcrt->next != NULL) pcrt = pcrt->next;

    lastnum = Curlisting->first + Curlisting->currecs - 1;
    if (lastnum == Curlisting->total) {
        if (Curlisting->curcrt->next == NULL) { utalarm (); return; }
        i = dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->fld_first),
                                                  firstvisi (pcrt->fld_first));
        Curlisting->curcrt = pcrt;
        Curlisting->row += i;
        crt2screen ();
        return;
    }

    Curlisting->makpage (lastnum, YES);

    Curlisting->curcrt = Curlisting->getlstcrt ();
    getcurcrt ();
    return;
}

void lspgup (void) {
    CRT   * pcrt = Curlisting->curcrt;
    int     i;

    while (pcrt->prev != NULL) pcrt = pcrt->prev;

    if (Curlisting->first == 1L) {
        if (Curlisting->curcrt->prev == NULL) { utalarm (); return; }
        i = dbcalcln (Cur_Edt, firstvisi (pcrt->fld_first),
                                     firstvisi (Curlisting->curcrt->fld_first));
        Curlisting->curcrt = pcrt;
        Curlisting->row -= i;
        crt2screen ();
        return;
    }

    Curlisting->makpage (Curlisting->first, NO);

    Curlisting->curcrt = Curlisting->getlstcrt ();
    getcurcrt ();
    return;
}

void lshome (void) {

    if (Curlisting->first == 1L) { lspgup (); return; }
    Curlisting->makpage (1L, YES);
    Curlisting->curcrt = Curlisting->getlstcrt ();
    Curlisting->row = Curlisting->firstrow;
    crt2screen ();
}

void lsend (void) {

    if (Curlisting->first + Curlisting->currecs - 1 == Curlisting->total)
        { lspgdn (); return; }

    Curlisting->makpage (Curlisting->total, NO);
    Curlisting->curcrt = Curlisting->getlstcrt ();
    Curlisting->row = Curlisting->firstrow;
    lspgdn ();
}

void lsremakpg (void) {
    CRT   * pcrt;
    int     i;

    i = 0;
    pcrt = Curlisting->curcrt;
    while (pcrt != NULL) {
        i++;
        pcrt = pcrt->prev;
    }

    Curlisting->makpage (Curlisting->first, YES);

    for (pcrt = Curlisting->getlstcrt (); i > 1 && pcrt->next != NULL; i--)
        pcrt = pcrt->next;

    Curlisting->curcrt = pcrt;
    Cur_Lst->cur->pfld = firstvisi (Curlisting->curcrt->fld_first);
    Cur_Lst->cur->line = Cur_Lst->cur->row = Curlisting->row;
    return;
}

int lsdelrec (void) {
    CRT   * pcrt;
    int     i, j, len, handle;
    long    offset, remain, numrec;

    pcrt = Curlisting->curcrt;
    i = 0;
    while (1) {
        pcrt = pcrt->prev;
        if (pcrt == NULL) break;
        i++;
    }
    j = 0;
    if (i == 0 && Curlisting->curcrt->next == NULL &&
                    Curlisting->first == Curlisting->total &&
                                                Curlisting->first > 1L)
        j = 1;

    if (Curlisting->lstfile) {
        handle = utopen (Curlisting->filename, UT_RW);
        offset = (Curlisting->first + i - 1) * sizeof (long);
        utlseek (handle, offset, UT_BEG);
        utread (handle, &numrec, sizeof (long));
        remain = utlseek (handle, 0L, UT_END) - sizeof (long) - offset;
        while (remain > 0L) {
            if (remain > PWRK_LEN) {
                remain -= PWRK_LEN;
                len = PWRK_LEN;
            } else {
                len = (int) remain;
                remain = 0L;
            }
            utlseek (handle, offset + sizeof (long), UT_BEG);
            utread (handle, Db_pwrk, len);
            utlseek (handle, offset, UT_BEG);
            utwrite (handle, Db_pwrk, len);
            offset += len;
        }
        utchsize (handle, offset);
        utclose (handle);
    } else numrec = Curlisting->curcrt->fld_first->val.lst.numrec;

    Cur_Lst->numrec = numrec;
    dbdelone (Cur_Lst);
    Curlisting->total--;
    if (Curlisting->total == 0L) return NO;

    Curlisting->makpage (Curlisting->first - j, YES);

    Curlisting->curcrt = Curlisting->getlstcrt ();
    Curlisting->row = Curlisting->firstrow;
    Curlisting->visicol = 0;
    Curlisting->hei =
        dbcalcln (Cur_Lst, firstvisi (Curlisting->curcrt->fld_first),
                            lastvisi (Curlisting->curcrt->fld_end)) + 1;
    crt2screen ();
    for (; i > 0; i--) {
        if (Curlisting->curcrt->next == NULL) break;
        lsdown ();
    }
    return YES;
}

static void getcurcrt (void) {
    int        H = Cur_Lst->pwin->img.dim.h;
    int        row, hei;

    row = Curlisting->firstrow;
    while (1) {
        hei = dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->fld_first),
                                    lastvisi (Curlisting->curcrt->fld_end));

        if (row >= Curlisting->row || row + hei >= Curlisting->row)  break;

        if (Curlisting->curcrt->next == NULL) break;
        row += hei + 1 + Curlisting->duphei;
        Curlisting->curcrt = Curlisting->curcrt->next;
    }
    if (row + hei > H - 1) {
        Curlisting->curcrt = Curlisting->curcrt->prev;
        row -= dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->fld_first),
                               firstvisi (Curlisting->curcrt->next->fld_first));
    }
    Curlisting->row = row;
    crt2screen ();
    return;
}

void crt2screen (void) {
    DOCEDT   * cur = Cur_Edt->cur;
    CRT      * pcrtcur, * pcrtprev, * pctmp;
    long       num;
    int        H = Cur_Lst->pwin->img.dim.h;
    int        hei, row;

    hei = Curlisting->hei =
            dbcalcln (Cur_Edt, firstvisi (Curlisting->curcrt->fld_first),
                                    lastvisi (Curlisting->curcrt->fld_end)) + 1;

    if (hei > H - 1 - Curlisting->firstrow) {
        num = Curlisting->first;
        pctmp = Curlisting->curcrt;
        while (1) {
            pctmp = pctmp->prev;
            if (pctmp == NULL) break;
            num++;
        }
        Curlisting->makpage (num, YES);
        Curlisting->row = Curlisting->firstrow;
        goto lab;
    }

    if (Curlisting->row < Curlisting->firstrow)
        Curlisting->row = Curlisting->firstrow;
    if (Curlisting->row + hei > H - 1) Curlisting->row = H - hei - 1;

    pcrtcur = Curlisting->curcrt; row = Curlisting->row;
    while (1) {
        pcrtprev = pcrtcur->prev;
        if (row == Curlisting->firstrow) break;
        if (pcrtprev == NULL) {
            if (!Curlisting->makcrt (NO)) break;
            pcrtprev = pcrtcur->prev;
        }
        row -= dbcalcln (Cur_Edt, firstvisi (pcrtprev->fld_first),
                                               firstvisi (pcrtcur->fld_first));
        if (row < Curlisting->firstrow) break;
        pcrtcur = pcrtprev;
    }

    if (pcrtprev != NULL) deload (pcrtprev, NO);

    while (1) {
        Curlisting->row = Curlisting->firstrow +
                        dbcalcln (Cur_Edt, firstvisi (pcrtcur->fld_first),
                                     firstvisi (Curlisting->curcrt->fld_first));

        if (Curlisting->row + Curlisting->hei <= H - 1) break;
        pcrtcur = pcrtcur->next;
        deload (pcrtcur->prev, NO);
    }

    row = Curlisting->row;
    pcrtcur = Curlisting->curcrt;
    hei = Curlisting->hei;
    while (1) {
        pctmp  = pcrtcur->next;
        if (row + hei > H - 1) break;
        if (pctmp == NULL) {
            if (!Curlisting->makcrt (YES)) break;
            pctmp  = pcrtcur->next;
        }
        row += hei + Curlisting->duphei;
        hei = dbcalcln (Cur_Edt, firstvisi (pctmp->fld_first),
                                            lastvisi (pctmp->fld_end)) + 1;
        pcrtcur = pctmp;
    }

    if (pctmp != NULL) deload (pctmp, YES);
lab:
    cur->pfld = firstvisi (Curlisting->curcrt->fld_first);
    cur->line = cur->row = Curlisting->row;

    return;
}

static void deload (CRT * pcrtfirst, bool down) {
    CRT     * pcrt = pcrtfirst;
    int       i = 0;

    while (pcrtfirst != NULL) {
        if (down) pcrtfirst = pcrt->next;
        else      pcrtfirst = pcrt->prev;
        i++;
        dbcrtdel (Cur_Edt, pcrt);
        pcrt = pcrtfirst;
    }
    Curlisting->currecs -= i;
    if (!down) Curlisting->first += i;
}

void updscreen (DOC * doc) {
    CELLSC   * pdata = doc->pwin->img.pdata;
    WINDOW   * pwin = doc->pwin;
    CRT      * pcrt;
    char far * pbuf, * pscreen;
    int        H = pwin->img.dim.h;
    int        W = pwin->img.dim.w, row, hei;
    byte       att;

    pcrt = Curlisting->getlstcrt ();
    row = Curlisting->firstrow;
    while (pcrt != NULL) {
        hei = dbcalcln (doc, firstvisi (pcrt->fld_first),
                                                 lastvisi (pcrt->fld_end)) + 1;
        if (pcrt == Curlisting->curcrt) {
            if (pcrt->opt.mark) att = pwin->attr.bold;
            else                att = pwin->attr.afld;
        } else if (pcrt->opt.mark) att = pwin->attr.nice;
        else goto nextcrt;

        if (row + hei > H) hei = H - row;
        if (hei <= 0) break;
        wnmovpa (pdata + row * W, hei * W, att);
nextcrt:
        row += hei + Curlisting->duphei;
        pcrt = pcrt->next;
    }

    pbuf    = (char far *) pdata;
    pscreen = wnviptrl (pwin->where_shown.row, pwin->where_shown.col);
    wnvicopy (&pbuf, &pscreen, H, W, NUM_COLS * 2, 0, Cmd[1]);
}

