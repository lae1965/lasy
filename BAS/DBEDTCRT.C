#include    <sldbas.h>

static  CRT   * getcrt       (CRT * pcrt);
static  void    jstmrkvfycur (int ii);

static  ITEM        noyes [] = {
                { " Нет ",        "НнYy",  NULL },
                { " Да ",         "ДдLl",  NULL }
};

void dbed_del (CRT * pcrt, bool ask) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pftmp;
    register  ii;

    if ((pcrt = getcrt (pcrt)) == NULL) return;
    if (pcrt->next != NULL)      pftmp = pcrt->fld_first;
    else if (pcrt->prev != NULL) pftmp = pcrt->prev->fld_first;
    else { utalarm (); return; }
    if (ask && mnwarnin (noyes, 2, 5, " ", "Вы уверены,", "что хотите удалить",
                                       "данную группу полей?"," ") != 1) return;

    ii = dbcalcln (doc, pftmp, cur->pfld);
    cur->pfld = dbcrtdel (doc, pcrt);
    if (cur->pfld == NULL) { currestore (); return; }
    cur->pfld = firstvisi (cur->pfld);

    jstmrkvfycur (-ii);

    return;
}

void dbed_mup (CRT * pcrt) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    register  ii, H = doc->pwin->img.dim.h;

    if (!dbsetfld ()) return;
    if ((pcrt = getcrt (pcrt)) == NULL) return;
    ii = dbcalcln (doc, pcrt->fld_first, cur->pfld);
    cur->pfld = dbcrtmul (doc, pcrt, NO, YES);
    if (cur->pfld == NULL) { Db_Err = NO_MEM; currestore (); return; }

    cur->pfld = firstvisi (cur->pfld);
    cur->line -= ii;
    if ((long)cur->row > cur->line) cur->row = (int) cur->line;
    utbound (cur->row, 0, H - 1);
    jstmrkvfycur (0);
    return;
}

void dbed_mdn (CRT * pcrt) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;

    if (!dbsetfld ()) return;
    if ((pcrt = getcrt (pcrt)) == NULL) return;
    cur->pfld = dbcrtmul (doc, pcrt, YES, YES);
    if (cur->pfld == NULL) { Db_Err = NO_MEM; currestore (); return; }
    cur->pfld = firstvisi (cur->pfld);
    jstmrkvfycur (dbcalcln (doc, Cur_Edt->old->pfld, cur->pfld));
    return;
}

void  dbed_srt (bool forward) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;
    FLD     * pfld = cur->pfld, * pftmp;
    long      i;

    if (!dbsetfld ()) return;

    pfld = firstvisi (dbsrtcrt (doc, pfld, forward));
    dbbeglen (doc, pfld, &cur->beg, &cur->len);
    doc->opt.needout = YES;
    pftmp = firstvisi (doc->crtroot->fld_first);
    i = dbgetpno (doc, pftmp) + dbcalcln (doc, pftmp, pfld);
    i -= cur->line;
    rowlinecorr ((int) i);
    doc->cur->pfld = pfld;
    cur2old ();
    val2pwrk ();
    return;
}

static  CRT * getcrt (CRT * pcrt1) {
    DOC       * doc = Cur_Edt;
    FLD       * pfld = doc->cur->pfld;
    char     ** ppname;
    CRT       * pcrt = pcrt1;
    register    total = 0, i;

    if (pcrt1 == NULL) pcrt = pfld->curcrt;
    if (doc->cur->pfld->opt.wrap && pcrt->opt.multy) pcrt = pcrt->highcrt;
    if (pcrt == doc->crtroot) { utalarm (); return NULL; }
    if (pcrt1 != NULL) return pcrt;
    pcrt1 = pcrt;
    while (pcrt != doc->crtroot) {
        pcrt = pcrt->highcrt;
        total++;
    }
    if ((ppname = calloc (total, sizeof (char *))) == NULL) return NULL;
    for (i = total - 1, pcrt = pcrt1; i >= 0; i--) {
        if ((ppname [i] = calloc (MAX_NAM_LEN + 6, sizeof (char))) == NULL)
            { pcrt = NULL; goto ret; }
        strcpy (ppname [i], "    ");
        strcat (ppname [i], (char *) doc->cnam [pcrt->idc].nam);
        pcrt = pcrt->highcrt;
    }
    if (pfld->opt.multy) strncpy (ppname [total - 1], " П:", 3);

    if ((i = mnrolmnu (WN_CENTER, WN_CENTER, total, total - 1, ppname,
                                                         NULL, 15, 4)) < 0)
        { pcrt = NULL; goto ret; }

    pcrt = pcrt1;
    i = total - i - 1;
    for (; i > 0; pcrt = pcrt->highcrt, i--);
ret:
    for (i = 0; i < total; i++) free (ppname [i]);
    free (ppname);
    return pcrt;
}

static  void jstmrkvfycur (int ii) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;

    doc->opt.needout = YES;
    doc->opt.change  = YES;
    rowlinecorr (ii);
    dbbeglen (doc, cur->pfld, &cur->beg, &cur->len);
    setnewfld ();
    if (cur->pfld->opt.protect) dbed_tab (NO);
    return;
}

