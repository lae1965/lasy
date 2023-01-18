#include    <sldbas.h>
#include    <alloc.h>



bool  dblodlst (int direct) {
    DOC             * basedoc;
    IDBASE          * pdb;
    CRT             * maincrt;
    char            * pname;
    bool              retcode = YES, down;
    int               handlst;
    long              numrec, offset;
    register          i = 0;

    if (idlst.total == 0L) return NO;

    if (direct == 1) {                            /* down */
        if (idlst.wasrecs + idlst.currecs == idlst.total)
            { utalarm (); return NO; }
        idlst.wasrecs += idlst.currecs;
    } else if (direct == -1) {                    /* up */
        if (idlst.wasrecs == 0L) { utalarm (); return NO; }
    }

    dbclrdoc (Cur_Lst, YES);
    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, Cur_Lst->opt._global);

    pdb = dbopendb (Cur_Lst->name_data, Cur_Lst->opt._global, UT_R);
    if (idlst.lstfile) handlst = utopen ("lst.tmp", UT_R);
    else {
        if      (direct ==  1) basedoc->numrec = idlst.last;      /* down   */
        else if (direct == -1) basedoc->numrec = idlst.first;     /* up     */
        else                   basedoc->numrec = idlst.first - 1; /* reread */
    }
    pname = (char *) basedoc->cnam [0].nam;
    maincrt = Cur_Lst->crtroot->lowcrt;

    if (direct == 0) direct = 1;
    down = (direct == 1);

    while ((strcmp (pname, (char *) Cur_Lst->cnam [maincrt->idc].nam)) != NULL)
        maincrt = maincrt->nextsis;

    while (1) {
        if (idlst.lstfile) {
            offset = (idlst.wasrecs + i * direct - (!down)) * sizeof (long);
            utlseek (handlst, offset, UT_BEG);
            utread (handlst, &numrec, sizeof (long));
        } else {
            numrec = basedoc->numrec + direct;
        }

        dblodrec (basedoc, pdb, numrec, down);
        dbcrtocr (Cur_Lst, basedoc, maincrt, basedoc->crtroot, NO);
        maincrt->fld_first->val.lst.numrec = basedoc->numrec;
        if (idlst.lstfile) maincrt->fld_first->val.lst.offset = offset;
        maincrt->fld_first->prnopt.full = YES;
        if (i == 0 && !idlst.lstfile) {
            if (down) idlst.first = basedoc->numrec;
            else      idlst.last  = basedoc->numrec;
        }
        i++;
        if (down) {
            if (idlst.wasrecs + i == idlst.total) break;
        } else {
            if (idlst.wasrecs - i == 0L) break;
        }
        if (Cur_Lst->total == idlst.H) break;
        if (idlst.H != -1 && Cur_Lst->total > idlst.H) {
            if (i > 1) {
                dbcrtdel (Cur_Lst, maincrt);
                i--;
                basedoc->numrec = numrec - direct;
            }
            break;
        }

        if (!utvfymem (40000L))  break;

        dbcrtmul (Cur_Lst, maincrt, down, NO);
        if (down) maincrt = maincrt->next;
        else      maincrt = maincrt->prev;
    }
    idlst.currecs = i;
    if (!down) idlst.wasrecs -= i;
    if (idlst.lstfile) utclose (handlst);
    else {
        if (down) idlst.last   = basedoc->numrec;
        else      idlst.first  = basedoc->numrec;
    }
    dbclosdb (pdb);
    dbdstroy (basedoc);
    return retcode;
}

bool    dbreldlst  (int change) {
    FLD     * pfld = Cur_Lst->cur->pfld;
    long      numrec, offset;
    int       idf, direct = 0, i, hand;


    if (change != 1) {
        while (pfld->curcrt->highcrt->idc != 0) pfld = pfld->prev;
        pfld = pfld->curcrt->fld_first;
        numrec = pfld->val.lst.numrec;
        offset = pfld->val.lst.offset;
        idf = pfld->idf;
    }
    if (change == -1) {
        idlst.total--;
        idlst.currecs--;
        if (idlst.total == 0L) return NO;
        if (idlst.currecs == 0) direct = -1;
        if (idlst.lstfile) {
            hand = utopen ("lst.tmp", UT_RW);
            offset = utlseek (hand, offset + sizeof (long), UT_BEG);
            while (1) {
                i = utread (hand, Db_pwrk, PWRK_LEN);
                if (i == 0) break;
                utlseek (hand, offset - sizeof (long), UT_BEG);
                utwrite (hand, Db_pwrk, i);
                offset = utlseek (hand, sizeof (long), UT_CUR);
            }
            utchsize (hand, offset - sizeof (long));
            utclose (hand);
        }
    } else if (change == 1 && !idlst.lstfile) idlst.total++;
lab:
    if (!dblodlst (direct)) return NO;
    dbnewdoc ();

    if (change == 0) {
        pfld = Cur_Lst->crtroot->fld_first;
        while (pfld->idf != idf) pfld = pfld->next;

        while (numrec != pfld->val.l) {
            pfld = nextcont (pfld, Cur_Lst->crtroot);
            if (pfld == NULL) { direct = 1; goto lab; }
        }
        pfld = firstvisi (pfld);
        while (Cur_Lst->cur->pfld != pfld) dbed_tab (NO);
    }
    return YES;
}

