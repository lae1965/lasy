#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <alloc.h>

#define     MAX_FILE_LEN    13

static bool     initto     (CRT * pcrt, char * basenam);
static bool     initfrom   (CRT * pcrt);
static double   cv_cmpval  (VAL * pv1, VAL * pv2, int valtype);
static int      ed_doc     (void);
static int      cv_v2c     (FLD * pfld, char * buf, int valtype);
static void     setno      (FLD * pfld);
static void     convert    (bool cross);
static void     rstpropt   (DOC * doc, bool prn, bool jst);
static char   * getdocnam  (CRT * pcrt);
static char   * getbasnam  (CRT * pcrt);
static bool     getglobal  (CRT * pcrt);
static bool     ischgprn   (CRT * pcrt);
static bool     ischgjst   (CRT * pcrt);
static bool     newbas2old (CRT * pcrt, char * basenam);

static DCB      dcb;
static char   * noyes  [] = { "Нет",    "Да"  };
static char   * result [] = { "Ошибка", "Ok!" };
static char   * pbuf;
static char     cvt [18 + MAX_NAM_LEN]   = "Конвертация базы ";
static int      numfrom;
static IDBASE * pidbfrom, * pidbto;
static DOC    * docfrom,  * docto;

static char * dockey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
"Enter    Select    CtrlF4/AltF4    Sort    F9     Delete     AltX     Cancel    ",
"XXXXXX            XXXXXXXXXXXXXX          XXXX              XXXXXX              "
};

void  main (void) {
    CRT       * pcrt;
    FLD       * pfld;
    IDEDT     * pidedt;
    char      * pfilnam;
    register    hand, handini;
    int         i;

    if ((pbuf = malloc (32000)) == NULL) return;
    if (!dbinidb (_PREVIOS, _PREVIOS, "LASY")) { free (pbuf); return; }

    Db_Fn_valtoc = cv_v2c;
    Db_Fn_cmpval = cv_cmpval;

    dbsl2nam (Db_ext [DB_DOC] + 1, 1, Db_Env);

    if ((pfilnam = dbfilnam ("Конвертор", Db_ext [DB_DOC]+1)) == NULL) goto ret;
    if ((Cur_Doc = dbinidoc (pfilnam, DB_VISI|DB_MAXWIN, DB_DOC, YES)) == NULL)
        goto ret;

    dbsl2nam (Db_ext [DB_DOC] + 1, 1, NULL);

    pfilnam = pbuf + MAX_NAM_LEN + 5;
    handini = utopen ("sld.ini", UT_R);
    utread (handini, &i, sizeof (int));
    pcrt = Cur_Doc->crtroot->lowcrt;
    pfld = firstvisi (pcrt->fld_first->next);
    setno (pfld);                             /* Шрифты */
    pfld = pfld->next;
    setno (pfld);                             /* Выравнивание */
    pfld = pfld->next->next;
    setno (pfld);                             /* Межбазовая реконструкция */
    Db_edit = YES;
    for (; i > 0; i--) {
        utread (handini, pbuf,    MAX_NAM_LEN + 1);
        utread (handini, pfilnam, MAX_FILE_LEN);
        hand = utopen (pfilnam, UT_R);
        utread (hand, &dcb, sizeof (DCB));
        utclose (hand);
        if (* dcb.name_data == EOS || !dcb.opt.fatherbas) continue;
        if ((hand = dbopen (dcb.name_data,DB_BAS,dcb.opt._global,UT_R)) == -1)
            continue;
        utclose (hand);

        if (numfrom > 0) {
            dbcrtmul (Cur_Doc, pcrt, YES, YES);
            pcrt = pcrt->next;
        }
        numfrom++;
        pfld = pcrt->fld_first;                   /* Имя документа */
        dbctoval (Cur_Doc, pfld, pbuf, &pfld->val);
        pfld = pfld->next;                        /* Имя файла документа */
        dbctoval (Cur_Doc, pfld, pfilnam, &pfld->val);
        pfld = pfld->next;                        /* Имя файла базы */
        dbctoval (Cur_Doc, pfld, dcb.name_data, &pfld->val);
        pfld = pfld->next;                        /* Глобальность базы */
        pfld->val.i = dcb.opt._global;
    }

    if (numfrom > 0) {
        dbsrtcrt (Cur_Doc, pcrt->fld_first, YES);
        dbdsplay (Cur_Doc, WN_NATIVE, WN_NATIVE, NULL);
        pidedt = utalloc (IDEDT);
        pidedt->funkey   = dockey;
        pidedt->prewait  = dbedstat;
        pidedt->postwait = ed_doc;
        pidedt->col      = -1;
        dbdocedt (Cur_Doc, pidedt, NO, YES);
        free (pidedt);
    } else
        mnwarnin (NULL, 0, 4, " ", "В текущем каталоге",
                                        "нет документов - родителей баз", " ");
    dbdstroy (Cur_Doc);
ret:
    utremove ("sld.ini");
    dbfindb ();
    free (pbuf);
    return;
}

static int ed_doc (void) {
    FLD       * pfld = Cur_Edt->cur->pfld;

    switch (Kb_sym) {
        case F9:
            dbed_del (pfld->curcrt, NO);
            numfrom--;
            break;
        case CtrlF4:        dbed_srt (YES);           break;
        case AltF4:         dbed_srt (NO);            break;
        case Esc:
        case AltX:          return 2;
        case Enter:
            switch (pfld->idf) {
                case  4:                      /* Шрифты */
                case  5:                      /* Выравниваение */
                case  7:                      /* Межбазовая реконструкция */
                    pfld->val.i = !pfld->val.i;
                    Cur_Edt->opt.needout = YES;
                    break;
                case  8:         convert (pfld->prev->val.i); break;
                case  9:         return  2;
                default :        break;
            }
            break;
        default :           return -2;
    }
    return 0;
}

static int  cv_v2c (FLD * pfld, char * buf, int valtype) {
    char * p;

    if (!pfld->prnopt.full) return 0;

    switch (valtype) {
        case 8:     p = noyes  [pfld->val.i];    break;
        case 9:     p = result [pfld->val.i];    break;
        default :                                return 0;
    }
    strcpy (buf, p);

    return (strlen (p));
}

static double cv_cmpval (VAL * pv1, VAL * pv2, int valtype) {

    if (valtype != 8 && valtype != 9) return 0.0;
    return ((double) (pv1->i - pv2->i));
}

static void  setno (FLD * pfld) {
    IDFLD   * pidf;

    pidf = dbgetidf (Cur_Doc, pfld);
    pfld->val.i = pidf->inival.i = 0;
    pfld->prnopt.full = pidf->prnopt.full = YES;

    return;
}

static void convert (bool cross) {
    CRT       * pcrtto = Cur_Doc->crtroot->lowcrt, * pcrtfrom;
    FLD       * pfld;
    char        basenam [5] = "_";
    long        total, numrec;
    bool        success;
    register    i, j;

    for (i = 0; i < numfrom; i++, pcrtto = pcrtto->next)
        pcrtto->fld_end->prnopt.full = NO;

    dboutdoc (Cur_Doc);

    pcrtto = Cur_Doc->crtroot->lowcrt;
    for (i = 0; i < numfrom; i++, pcrtto = pcrtto->next) {
        success = YES;
        pidbto = pidbfrom = NULL;
        docto  = docfrom  = NULL;
        utl2crdx ((long) i, basenam + 1, 0, 10);
        if (!initto (pcrtto, basenam + 1)) { success = NO; goto err; }

        numrec = 0L; total = -1L;
        do {
            if (cross) { pcrtfrom = Cur_Doc->crtroot->lowcrt; j = 0; }
            else       { pcrtfrom = pcrtto;  j = numfrom - 1; }
            numrec++;
            for (; j < numfrom; j++, pcrtfrom = pcrtfrom->next) {
                pidbfrom = NULL; docfrom  = NULL;

                if (!initfrom (pcrtfrom))  { success = NO; goto err; }
                if (total == -1L) {
                    total = pidbfrom->slhd.total;
                    if (total ==  0L) goto err;
                    utmovabs (cvt + 17, pcrtto->fld_first->val.s.p,
                                                               MAX_NAM_LEN, 0);
                    wndiswrk (total * (cross) ? numfrom : 1, 3, " ", cvt, " ");
                }

                docto->numrec = 0L;
                if (!dblodrec (docfrom, pidbfrom, numrec, YES))
                    { success = NO; goto err; }

                numrec = docfrom->numrec;

                if (!dbcrtocr (docto, docfrom, docto->crtroot,
                                                        docfrom->crtroot, NO))
                    { success = NO; goto err; }

                wnupdwrk (1L);

                dbclosdb (pidbfrom); pidbfrom = NULL;
                dbdstroy (docfrom);  docfrom  = NULL;
            }
            rstpropt (docto, ischgprn (pcrtto), ischgjst (pcrtto));
            if (!dbsavtmp (docto, docto->crtroot, pidbto))
                { success = NO; goto err; }
            dbclrdoc (docto, NO);
            total--;
        } while (total > 0L);
err:
        dbclosdb (pidbfrom);
        dbdstroy (docfrom);
        dbdstroy (docto);
        dbclosdb (pidbto);

        if (!cross) {
            if (success) success = newbas2old (pcrtto, basenam);
            if (!success)  dbdelbas (basenam + 1, NO, NO);
        }

        wnremwrk ();

        pfld = pcrtto->fld_end;           /* Результат */
        pfld->val.i = success;
        pfld->prnopt.full = YES;
        dboutdoc (Cur_Doc);
        if (cross && !success) break;
    }
    if (!cross) return;

    if (success) {
        pcrtto = Cur_Doc->crtroot->lowcrt;
        for (i = 0; i < numfrom; i++, pcrtto = pcrtto->next) {
            utl2crdx ((long) i, basenam + 1, 0, 10);
            if (!(success = newbas2old (pcrtto, basenam))) break;
        }
    }

    if (!success) {
        pcrtto = Cur_Doc->crtroot->lowcrt;
        for (i = 0; i < numfrom; i++, pcrtto = pcrtto->next) {
            dbdelbas (basenam + 1, NO, NO);
            pfld = pcrtto->fld_end;
            pfld->val.i = 0;
            pfld->prnopt.full = YES;
        }
        dboutdoc (Cur_Doc);
    }
    return;
}

static bool newbas2old (CRT * pcrt, char * basenam) {

    dbrenambas (getbasnam (pcrt), getglobal (pcrt),
                                                basenam, getglobal (pcrt), NO);

    if (dbmovbas (basenam + 1, NO, getbasnam (pcrt), getglobal (pcrt), NO))
        dbdelbas (basenam, getglobal (pcrt), NO);
    else {
        dbrenambas (basenam, getglobal(pcrt),
                                         getbasnam(pcrt), getglobal(pcrt), NO);
        return NO;
    }
    return YES;
}

static bool initto  (CRT * pcrt, char * basenam) {
    bool        retcode = YES;

    if ((pidbto=dbcreatbas (getdocnam (pcrt), NO, DB_DOC, basenam, NO))==NULL)
        return NO;
    utclose (pidbto->handb);
    if ((docto = dbinidoc (basenam, 0, DB_BAS, NO)) == NULL)
        { retcode = NO; goto ret; }
    if ((pidbto->handb = dbopen (basenam, DB_BAS, NO, UT_W)) == -1)
        retcode = NO;
ret:
    return  retcode;
}

static bool initfrom (CRT * pcrt) {
    bool        retcode = YES;

    if ((docfrom = dbinidoc (getbasnam (pcrt), 0, DB_BAS, getglobal (pcrt)))
                                                                       == NULL)
        { retcode = NO; goto ret; }
    if ((pidbfrom = dbopendb (getbasnam (pcrt), getglobal (pcrt), UT_R))
                                                                       == NULL)
        retcode = NO;
ret:
    return retcode;
}

static void rstpropt (DOC * doc, bool prn, bool jst) {
    FLD     * pfld = doc->crtroot->fld_first;
    PRNOPT  * fldprnopt, * idfprnopt;

    if (!prn && !jst) return;

    do {
        fldprnopt = &pfld->prnopt;
        idfprnopt = &((dbgetidf (doc, pfld))->prnopt);
        if (prn) {
            fldprnopt->nlq    = idfprnopt->nlq;
            fldprnopt->italic = idfprnopt->italic;
            fldprnopt->under  = idfprnopt->under;
            fldprnopt->bold   = idfprnopt->bold;
            fldprnopt->elite  = idfprnopt->elite;
            fldprnopt->size   = idfprnopt->size;
            fldprnopt->script = idfprnopt->script;
        }
        if (jst)  fldprnopt->jst = idfprnopt->jst;
        pfld = pfld->next;
    } while (pfld != NULL);
    return;
}

static char * getdocnam (CRT * pcrt) {
    byte    * p;

    p = (dbgetfld (Cur_Doc, pcrt->fld_first, "Имя файла документа", YES))->val.s.p;
    return (char *) p;
}

static char * getbasnam (CRT * pcrt) {
    byte    * p;

    p = (dbgetfld (Cur_Doc, pcrt->fld_first, "Имя файла базы", YES))->val.s.p;
    return (char *) p;
}

static bool   getglobal (CRT * pcrt) {

    return (dbgetfld (Cur_Doc, pcrt->fld_first, "Глобальность базы", YES))->val.i;
}

static bool   ischgprn (CRT * pcrt) {

    return (dbgetfld (Cur_Doc, pcrt->fld_first, "Шрифты", YES))->val.i;
}

static bool   ischgjst (CRT * pcrt) {

    return (dbgetfld (Cur_Doc, pcrt->fld_first, "Выравнивание", YES))->val.i;
}

