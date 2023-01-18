/******* * 09/24/91 01:44pm **************************************************
           09/20/94 12:33pm - revised

        Функция   DBINIDOC  - инициализирует пользовательский документ

     DOC  * dbinidoc (int hand, int opt, int doctype);

        int     hand    - Handle файла с прототипом документа;
        int     opt     - Опции для инициализации документа;
                          Параметр может иметь следующие значения:
                            DB_VISI    - документ должен содержать необходимые
                                         структуры для визуализации и структуры
                                         полей для манипулирования их
                                         содержимым; Если данное значение opt
                                         не задано, то это означает, что
                                         документ должен содержать только
                                         структуру полей для манипуляции их
                                         содержимым, при этом структуры для
                                         визуализации не создаются;
                            DB_MAXWIN  - окно для видимого документа создается
                                         размерами:
                                         col х (_MAXROWS - (dcb.row + 1));

        int     doctype - Тип инициализируемого документа.


        Возвращает:                      Указатель на созданный документ или
                                         NULL, если ошибка;
 *****************************************************************************/

#include <sldbas.h>
#include <alloc.h>

DOC     * Cur_Doc, * Cur_Lst, * Cur_Qry, * Cur_Edt;

static  DCB       dcb;

static DOC  * _inidoc (char * pfilnam, int opt, int doctype, bool db_global);
static bool   reload (int hand, PNSWAP * pswap);

DOC  * dbinidoc (char * pfilnam, int opt, int doctype, bool db_global) {
    DOC     * doc;
    IDCDF   * pidcdf;
    char    * pname;
    register  i = 0, hand;

    if ((doc = _inidoc (pfilnam, opt, doctype, db_global)) == NULL) return NULL;
    if (doctype == DB_BAS || doctype == DB_QRYWRK) return doc;

    for (; i < doc->numidf; i++) {
        if (doc->idf [i].initype == SL_DICT) {
            pname = (char *) doc->dnam [doc->idf [i].fldlink.docnam].nam;
            pidcdf = Db_topidcdf;
            while (pidcdf != NULL) {
                if (strcmp ((char *) pidcdf->doc->cnam [0].nam, pname) == 0)
                    goto lab;
                pidcdf = pidcdf->next;
            }
            pidcdf = utalloc (IDCDF);
            pfilnam = dbfilnam (pname, Db_ext [DB_DICT] + 1);
            pidcdf->doc = _inidoc (pfilnam, DB_VISI, DB_DICT, NO);
            if (Db_topidcdf == NULL)
                Cdf_Wind = wncreate (_MAXROWS - 7, NUM_COLS - 4, 0, 0,
                                                           &dcb.bor, &dcb.att);
            pidcdf->doc->pwin = Cdf_Wind;
            pidcdf->next = Db_topidcdf;
            Db_topidcdf = pidcdf;

            if ((hand = dbopen (pidcdf->doc->name_data, DB_VOC,NO,UT_R)) == -1)
                goto lab;
            dbloddoc (pidcdf->doc, hand);
            utclose (hand);
lab:
            pidcdf->num++;
            doc->idf [i].fldlink.link.pdoc = pidcdf->doc;
        }
    }
    return doc;
}

static DOC  * _inidoc (char * pfilnam, int opt, int doctype, bool db_global) {
    DOC     * doc;
    VAL     * pval;
    register  i, hand;
    word      j = (word) 65536L;
    bool      init;

    init = (doctype != DB_QRY && doctype != DB_BAS && doctype != DB_QRYWRK);
    if ((doc = utalloc (DOC)) == NULL) goto badmem;

    utmovabs (doc->name_doc, pfilnam, 8, 0);

    if ((hand = dbopen (pfilnam, doctype, db_global, UT_R)) == -1) goto badmem;

    if (utread (hand, &dcb, sizeof (DCB)) == j) goto badmem;

    utmovabs (doc->name_data, dcb.name_data, 8, 0);
    doc->opt._global = dcb.opt._global;

    if ((doc->cnam = malloc (dcb.cnamlen)) == NULL) goto badmem;
    if (utread (hand, doc->cnam, dcb.cnamlen) == j) goto badmem;

    if ((doc->fnam = malloc (dcb.fnamlen)) == NULL) goto badmem;
    if (utread (hand, doc->fnam, dcb.fnamlen) == j) goto badmem;

    if (dcb.tnamlen > 0) {
        if ((doc->tnam = malloc (dcb.tnamlen)) == NULL) goto badmem;
        if (utread (hand, doc->tnam, dcb.tnamlen) == j) goto badmem;
    }

    if (dcb.dnamlen > 0) {
        if ((doc->dnam = malloc (dcb.dnamlen)) == NULL) goto badmem;
        if (utread (hand, doc->dnam, dcb.dnamlen) == j) goto badmem;
    }

    doc->numidc = dcb.idclen / sizeof(IDCRT);
    if ((doc->idc = malloc (dcb.idclen)) == NULL) goto badmem;
    if (utread (hand, doc->idc, dcb.idclen) == j) goto badmem;

    doc->numidf = dcb.idflen/sizeof(IDFLD);
    if ((doc->idf = malloc (dcb.idflen)) == NULL) goto badmem;
    if (utread (hand, doc->idf, dcb.idflen) == j) goto badmem;

    if (opt & DB_VISI) {
        doc->opt.visibl = YES;
        if ((doc->idl = malloc (dcb.idllen)) == NULL) goto badmem;
        if (utread (hand, doc->idl, dcb.idllen) == j) goto badmem;
        if ((doc->pwrk = malloc (PAGE_SIZE)) == NULL) goto badmem;
        doc->d.total  = dcb.pndlen / sizeof (int);
        if (doc->d.total > 0 && !reload (hand, &doc->d)) goto badmem;
        doc->o.total  = dcb.pnolen / sizeof (int);
        if (!reload (hand, &doc->o)) goto badmem;
        if (doctype != DB_DICT) {
            if (opt & DB_MAXWIN) {
                i = _MAXROWS - (dcb.row + 1);
                doc->opt.maxwin = YES;
            } else  i = dcb.h;
            if ((doc->pwin = wncreate (i, dcb.w, dcb.row, dcb.col, &dcb.bor,
                                                             &dcb.att)) == NULL)
                goto badmem;
            dbsethei (doc);
        }
        doc->prnopt = dcb.prnopt;
        doc->prnpag = dcb.prnpag;
    }
    doc->doctype = doctype;
    utlseek (hand, dcb.inioff, UT_BEG);
    for (i = 0; i < doc->numidf; i++) {
        if (!(opt & DB_VISI)) doc->idf [i].opt.first = NO;
        pval = &doc->idf [i].inival;
        if (init) {
            if (doc->idf [i].valtype == SL_TEXT && pval->s.len > 0) {
                if ((pval->s.p = calloc (pval->s.len + 1, 1)) == NULL)
                    goto badmem;
                utread   (hand, Db_pwrk, pval->s.len * 2);
                utdecode (pval->s.p, Db_pwrk, pval->s.len);
            }
        } else pval->d = 0.0;
    }

    strcpy (doc->sign, DC_SIGN);

    if (!dbclrdoc (doc, init)) goto badmem;

    utclose (hand);
    return doc;
badmem:
    dbdstroy (doc);
    return NULL;
}

bool dbclrdoc (DOC * doc, bool init) {

    if (doc->crtroot != NULL) {
        dbcrtdel (doc, doc->crtroot);
        mnfrebrk (&doc->prnpag.firstbrk);
    }
    if (!dbmkroot (doc, init)) return NO;
    return YES;
}

static bool  reload (int hand, PNSWAP * pswap) {
    register        tsize;

    if ((pswap->pn = malloc (pswap->total * sizeof (int))) == NULL) return NO;
    utread (hand, pswap->pn, pswap->total * sizeof (int));

    tsize = pswap->pn [pswap->total - 1];
    if ((pswap->ptxt = malloc (tsize)) == NULL)
        { utfree (&pswap->pn); return NO; }
    utread (hand, pswap->ptxt, tsize);

    return YES;
}

