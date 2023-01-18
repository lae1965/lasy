#include    <sldbas.h>
#include    <string.h>
#include    <alloc.h>


int dbcalcln (DOC * doc, FLD * pfcur, FLD * pflast) {
    FLD       * pftmp;
    register    i = 0;

    pfcur  = firstvisi (pfcur);
    pflast = lastvisi  (pflast);

    if (pfcur == pflast) return 0;

    while (1) {
        pftmp = next (pfcur);
        i += how_row (doc, pfcur, pftmp);
        if (pftmp == pflast) return i;
        pfcur = pftmp;
    }
}

void  dbbeglen (DOC * doc, FLD * pfld, int * b, int * l) {
    int       beg = 0, len = 0;

    if (doc->opt.visibl && pfld->opt.visibl)
        dbfindbr (dbgetstr (doc, pfld), &beg, &len, pfld->instr);
    else len = 32500;

    if (b != NULL) * b = beg;
    if (l != NULL) * l = len;
    return;
}

bool    dbcor_bl (int visicol, int W, int * b, int * l) {
    register    beg, end;

    beg = * b;                  end = beg + * l;
    W += visicol;
    if (end <= visicol || beg >= W) return NO;            /* поле не видно */

    beg = max (beg, visicol);   end = min (end, W);
    * b = beg - visicol;       * l = end - beg;
    return YES;
}

byte   * dbgetstr (DOC * doc, FLD * pfld) {
    IDLIN   * pidd;
    register  npn;

    while (pfld->instr != 1) pfld = pfld->prev;

    if (pfld->idc != 0) {
        pidd = dbgetidd (doc, pfld);
        npn = pidd->npn;
        return ((byte *) (doc->d.ptxt + doc->d.pn [npn + pidd->num - 1]));
    }
    npn = dbgetpno (doc, pfld);
    return ((byte *) (doc->o.ptxt + doc->o.pn [npn]));
}

int      dbvaltoc (DOC * doc, FLD * pfld, byte ** pp, int wid) {
    IDFLD       * pidf;
    VAL         * pval;
    register      vallen = 0, valtype;
    byte        * pbuf = * pp;

    pidf = dbgetidf (doc, pfld);
    if (doc->doctype == DB_QRY) valtype = SL_TEXT;
    else                        valtype = pidf->valtype;
    switch (valtype) {
        case SL_CDF:
            if (!pfld->prnopt.full ||
                    (pval = dbcdf2nam (pidf->fldlink.link.pdoc,
                                                         pfld->val.i)) == NULL)
                return 0;
            vallen = pval->s.len;
            pbuf   = pval->s.p;
            break;
        case SL_TEXT:
            vallen = pfld->val.s.len;
            pbuf   = pfld->val.s.p;
            break;
        case SL_FLOAT:
            if (!pfld->prnopt.full) return 0;
            if (pidf->valopt.d.opt & SL_EXP)
                vallen = utdbl2ce ((double) pfld->val.f, (char *) pbuf,
                                    pidf->valopt.d.len, pidf->valopt.d.opt);
            else
                vallen = utdbl2cf ((double) pfld->val.f, (char *) pbuf, wid,
                                    pidf->valopt.d.len, pidf->valopt.d.opt);
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            if (!pfld->prnopt.full) return 0;
            if (pidf->valopt.d.opt & SL_EXP)
                vallen = utdbl2ce (pfld->val.d, (char *) pbuf,
                                     pidf->valopt.d.len, pidf->valopt.d.opt);
            else
                vallen = utdbl2cf (pfld->val.d, (char *) pbuf, wid,
                                       pidf->valopt.d.len, pidf->valopt.d.opt);

            break;
        case SL_DATE:
            if (!pfld->prnopt.full) return 0;
            vallen = utdate2c (pfld->val.l, (char *) pbuf,
                             pidf->valopt.date.separ, pidf->valopt.date.opt);
            break;
        case SL_LONG:
            if (!pfld->prnopt.full) return 0;
            vallen = utl2c (pfld->val.l, (char *) pbuf, pidf->valopt.d.opt);
            break;
        case SL_INT:
            if (!pfld->prnopt.full) return 0;
            vallen = utl2c ((long)pfld->val.i,(char *)pbuf,pidf->valopt.d.opt);
            break;
        default :
            if (Db_Fn_valtoc != NULL)
                vallen = (* Db_Fn_valtoc) (pfld, (char *) pbuf, valtype);
            break;
    }
    * pp = pbuf;
    return vallen;
}

int      dbctoval (DOC * doc, FLD * pfld, void * pbuf, VAL * pval) {
    IDFLD  * pidf;
    register len, valtype;
    int      ret = FLD_IS_VAL;
    bool     optfull = pfld->prnopt.full;
    long     ltmpnum = 0L;
    double   dtmpnum = 0.0;

    if (!Db_edit) return 0;
    pfld->prnopt.full = NO;
    pidf = dbgetidf (doc, pfld);
    if (doc->doctype == DB_QRY) valtype = SL_TEXT;
    else                        valtype = pidf->valtype;
    switch (valtype) {
        case SL_CDF:
        case SL_TEXT:
            utfree (&pval->s.p);
            len = strlen ((char *) pbuf);
            while (((char *) pbuf) [len - 1] == ' ' && len > 0) len--;
            if (len != 0) {
                if ((pval->s.p = malloc (len + 1)) == NULL) return NO_MEM;
                utmovabs ((char far *) pval->s.p, pbuf, len, 0);
                pval->s.p [len] = '\0';
                pfld->prnopt.full = YES;
            }
            pval->s.len = len;
            break;
        case SL_FLOAT:
            if (!utisempt ((byte *)pbuf)) {
                if (!utc2dbl (&(char *) pbuf, &dtmpnum, NO))
                    { ret = BAD_VAL; break; }
                if (!utisempt ((byte *)pbuf))  { ret = DIRTY_VAL; break; }
                if (utrange ((float) dtmpnum, pidf->dnval.f, pidf->upval.f))
                    { ret = NO_RANGE; break; }
                pfld->prnopt.full = YES;
            }
            pval->f = (float) dtmpnum;
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            if (!utisempt ((byte *)pbuf)) {
                if (!utc2dbl (&(char *) pbuf, &dtmpnum, YES))
                    { ret = BAD_VAL; break; }
                if (!utisempt ((byte *)pbuf)) { ret = DIRTY_VAL; break; }
                if (utrange (dtmpnum, pidf->dnval.d, pidf->upval.d))
                    { ret = NO_RANGE; break; }
                pfld->prnopt.full = YES;
            }
            pval->d = dtmpnum;
            break;
        case SL_DATE:
            if (!utisempt ((byte *)pbuf)) {
                if ((ltmpnum = utc2date (&(char *) pbuf,
                                           pidf->valopt.date.centure)) == -1L)
                    { ret = BAD_VAL; break; }
                if (!utisempt ((byte *)pbuf))  { ret = DIRTY_VAL; break; }
                if (utrange (ltmpnum, pidf->dnval.l, pidf->upval.l))
                    { ret = NO_RANGE; break; }
                pfld->prnopt.full = YES;
            }
            pval->l = ltmpnum;
            break;
        case SL_LONG:
            if (!utisempt ((byte *)pbuf)) {
                if (!utc2lrdx (&(char *)pbuf, &ltmpnum, 10))
                    { ret = BAD_VAL; break; }
                if (!utisempt ((byte *)pbuf))  { ret = DIRTY_VAL; break; }
                if (utrange (ltmpnum, pidf->dnval.l, pidf->upval.l))
                    { ret = NO_RANGE; break; }
                pfld->prnopt.full = YES;
            }
            pval->l = ltmpnum;
            break;
        case SL_INT:
            if (!utisempt ((byte *)pbuf)) {
                if (!utc2lrdx (&(char *)pbuf, &ltmpnum, 10))
                    { ret = BAD_VAL; break; }
                if (!utisempt ((byte *)pbuf))  { ret = DIRTY_VAL; break; }
                if (utrange (ltmpnum,(long)pidf->dnval.i,(long)pidf->upval.i))
                    { ret = NO_RANGE; break; }
                pfld->prnopt.full = YES;
            }
            pval->i = (int) ltmpnum;
            break;
        default :
            if (Db_Fn_ctoval == NULL) pfld->prnopt.full = optfull;
            else  ret = (* Db_Fn_ctoval) (pfld, (char *) pbuf, valtype);
            break;
    }
    return ret;
}

FLD   * prev (FLD * pcur) {

    do   pcur = pcur->prev;
    while (pcur != NULL && !pcur->opt.visibl);

    return pcur;
}

FLD   * next (FLD * pcur) {

    do   pcur = pcur->next;
    while (pcur != NULL && !pcur->opt.visibl);

    return pcur;
}

FLD * firstvisi (FLD * pffirst) {

    if (!pffirst->opt.visibl) pffirst = next (pffirst);
    return pffirst;
}

FLD * lastvisi (FLD * pflast) {

    if (!pflast->opt.visibl) pflast = prev (pflast);
    return pflast;
}

int    dbgetpno (DOC * doc, FLD * pfld) {
    return ((doc->idl + (doc->idf + pfld->idf)->idl)->npn);
}

int    dbgetpnd (DOC * doc, FLD * pfld) {
    return ((dbgetidd (doc, pfld))->npn);
}

IDLIN   * dbgetidl (DOC * doc, FLD * pfld) {
    return (doc->idl + (doc->idf + pfld->idf)->idl);
}

IDLIN   * dbgetidd (DOC * doc, FLD * pfld) {
    return (&doc->idc [pfld->idc].idd);
}

IDFLD   * dbgetidf (DOC * doc, FLD * pfld) {
    return (doc->idf + pfld->idf);
}

void   dbpr2par (FLD * pfld) {
    PRNOPT  prnopt = pfld->prnopt;
    CRT   * pcrt = pfld->curcrt;

    if (!pfld->opt.par) while ((pcrt = pcrt->prev) != NULL) {
        pcrt->fld_first->prnopt = prnopt;
        if (pcrt->fld_first->opt.par) break;
    }

    pcrt = pfld->curcrt;
    while ((pcrt = pcrt->next) != NULL && !pcrt->fld_first->opt.par)
        pcrt->fld_first->prnopt = prnopt;

    return;
}

void   dbsetjst (FLD * pfld, int jst) {
    CRT   * papa;
    FLD   * pftmp;

    if (!pfld->opt.wrap) { pfld->prnopt.jst = jst; return; }

    Cur_Edt->opt.needout = YES;
    papa = dbgetpapa (pfld);

    while (!pfld->opt.par && (pftmp = prevcont (pfld, papa)) != NULL)
        pfld = pftmp;

    while (1) {
        pfld->prnopt.jst = jst;
        pftmp = nextcont (pfld, papa);
        if (pftmp == NULL || pftmp->opt.par) return;
        pfld = pftmp;
    }
}

CRT  * dbgetpapa (FLD * pfld) {
    CRT * papa = pfld->curcrt;

    if (papa->opt.multy) papa = papa->highcrt;
    return papa;
}

FLD * nextcont (FLD * pfld, CRT * papa) {
    register    idn = pfld->idn;

    while (1) {
        if (pfld == papa->fld_end) return NULL;
        pfld = pfld->next;
        if (pfld->idn == idn) return pfld;
    }
}

FLD * prevcont (FLD * pfld, CRT * papa) {
    int    idn = pfld->idn;

    while (1) {
        if (pfld == papa->fld_first) return NULL;
        pfld = pfld->prev;
        if (pfld->idn == idn) return pfld;
    }
}

void    dbclrdone (CRT * pcrt) {
    FLD    * pfld  = pcrt->fld_first;
    FLD    * pfend = pcrt->fld_end;

    while (1) {
        pfld->opt.done = NO;
        if (pfld == pfend) return;
        pfld = pfld->next;
    }
}

int     how_row (DOC * doc, FLD * high, FLD * low) {
    register    i;

    if (low->instr > high->instr) return 0;

    if (low->idc == 0) i = (int) (dbgetpno (doc, low) - dbgetpno (doc, high));
    else               i = (dbgetidd (doc, low))->num;

    return i;
}

int     dbcdf2cod (DOC * doc, byte * pname) {
    CRT * pcrt = doc->crtroot->lowcrt;

    while (pcrt != NULL) {
        if (dbcmpsmp (pname, pcrt->fld_first->val.s.p) == 0)
            return pcrt->fld_last->val.i;
        pcrt = pcrt->next;
    }
    return -1;
}

VAL * dbcdf2nam (DOC * doc, int code) {
    CRT * pcrt = doc->crtroot->lowcrt;

    while (pcrt != NULL) {
        if (code == pcrt->fld_last->val.i)
            return &pcrt->fld_first->val;
        pcrt = pcrt->next;
    }
    return NULL;
}

long    getlastnumrec (void) {
    IDBASE      * pdb;
    long          lastnumrec;

    pdb = dbopendb (Cur_Doc->name_data, Cur_Doc->opt._global, UT_R);
    lastnumrec = (pdb->lenh - sizeof (SLHD)) / sizeof (long);
    dbclosdb (pdb);
    return lastnumrec;
}

void    dbdsplay (DOC * doc, int row, int col, char * pttl) {
    WINDOW    * pwin = doc->pwin;
    PIMAGE    * pimg = &pwin->img;
    int         i, j, bord, shadow;

    if (pwin->options.shown)  return;

    if (doc->doctype == DB_DICT) i = doc->crtroot->fld_first->val.i;
    else if (row == DB_CENTER)   i = doc->total;
    else goto lab;

    bord = ((pwin->bord.type & BORD_TYPE) > 0) * 2;
    shadow = ((pwin->bord.type & BORD_SHADOW) > 0);
    j = NUM_ROWS - 3 - bord - shadow;
    if (i > j) i = j;
    if (i == 0) i = NUM_ROWS - 6;
    pwin->img.dim.h = i;
    pwin->where_shown.row = (NUM_ROWS - 3 - (i + bord + shadow)) / 2 + 3;
    row = WN_NATIVE;

    if (doc->doctype == DB_DICT) {
        dbbeglen (doc, doc->crtroot->fld_first->next, NULL, &i);
        i += 2;
        if (i > NUM_COLS - 4) i = NUM_COLS - 4;
        pimg->dim.w = i;
    }
lab:
    wnmovpca (pimg->pdata, ' ', pimg->dim.h * pimg->dim.w, pwin->attr.text);

    if (pttl != NULL) {
        pwin->bord.pttitle = pttl;
        pwin->bord.type |= BORD_TCT;
    }

    wndsplay (pwin, row, col);
    return;
}

void    dbremove (DOC * doc) {
    WINDOW * pwin = doc->pwin;

    mnfrebrk (&doc->prnpag.firstbrk);
    if (wnvalwin (pwin)) {
        pwin->bord.pttitle = NULL;
        pwin->bord.type = utclrbit (pwin->bord.type, BORD_TCT);
        wnremove (pwin);
    }
    return;
}

FLD * dbgetfld (DOC * doc, FLD * pfld, char * pname, bool forward) {

    while (pfld != NULL) {
        if (strcmp (pname, (char *) doc->fnam [pfld->idn].nam) == 0) break;
        if (forward) pfld = pfld->next;
        else         pfld = pfld->prev;
    }
    return pfld;
}

CRT * dbgetcrt (DOC * doc, CRT * pcrt, char * pname) {
    CRT     * pcrt1;

    while (pcrt != NULL) {
        if (strcmp (pname, (char *) doc->cnam [pcrt->idc].nam) == 0) break;
        if (pcrt->lowcrt != NULL) {
            pcrt1 = dbgetcrt (doc, pcrt->lowcrt, pname);
            if (pcrt1 != NULL) return pcrt1;
        }
        pcrt = pcrt->nextsis;
    }
    return pcrt;
}

void    dbsethei (DOC * doc) {
    WINDOW * pwin = doc->pwin;

    if (!doc->opt.maxwin) return;

    pwin->img.dim.h = NUM_ROWS - (pwin->where_shown.row + 1 +
                            ((pwin->bord.type & BORD_TYPE) > 0) +
                                        ((pwin->bord.type & BORD_SHADOW) > 0));
    return;
}

void dbmkname (char * fullname, char * name, int doctype, bool _global) {

    if (_global) strcpy (fullname, Db_Env);
    else         * fullname = EOS;

    strcat  (fullname, name);
    utchext (fullname, Db_ext [doctype]);
    return;
}

