#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <slmous.h>
#include    <slmenu.h>
#include    <stdlib.h>
#include    <alloc.h>


static char   docname [70], olddoc [MAX_NAM_LEN+1], curname [MAX_NAM_LEN+1];
static char   oldqry [MAX_NAM_LEN+1];

BORDER        mnubrd  = { BORD_SHADOW,           NULL, NULL, NULL };
BORDER        vmnubrd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };

static ITEM   itm[] = {
                { " Загрузить",      "ЗзPp", NULL },
                { " Поиск ",         "ПпGg", NULL },
                { " Редактор",       "РрHh", NULL },
                { " Просмотр",       "МмVv", NULL },
                { " Сохранить",      "СсCc", NULL },
                { " Удалить ",       "УуEe", NULL },
                { " Очистить",       "ОоJj", NULL },
                { " Список ",        "ИиBb", NULL },
                { " Выход ",         "ВвDd", NULL }
};

static ITEM   savitm[] = {
            { " В базу             ", "ЗзPp", NULL },
            { " В произвольный файл", "ФфAa", NULL }
};

static char * viewkey [] = {
"1       2       3       4       5       6       7Print  8       9       10      ",
"1       2       3       4       5ToFile 6       7PrnFld 8       9       10NxtRec",
"1       2       3       4       5       6       7PrnDoc 8       9       10DelRec",
"1       2       3       4       5       6       7PrnPar 8       9       10PrvRec",
"X      XX      XX      XX      XX      XX      XX      XX      XX      XXX      "
};

static char * dockey [] = {
"1FmtPar 2SgDbEr 3ParSwt 4AutJmp 5Today  6Crt  \30 7Print  8Crt  \31 9DelCrt 10SavRec",
"1       2Ed/Mov 3       4Jstify 5ToFile 6CrtHi\30 7PrnFld 8CrtHi\31 9DelCHi 10NxtRec",
"1       2       3       4       5       6       7PrnDoc 8       9ClrDoc 10DelRec",
"1       2       3       4       5       6       7PrnPar 8       9       10PrvRec",
"X      XX      XX      XX      XX      XX      XX      XX      XX      XXX      "
};

static char * qrykey [] = {
"1       2       3       4AutJmp 5Today  6Crt  \30 7Print  8Crt  \31 9DelCrt 10Find  ",
"1       2       3       4Jstify 5ToFile 6CrtHi\30 7PrnFld 8CrtHi\31 9DelCHi 10      ",
"1       2       3       4       5       6       7PrnDoc 8       9ClrDoc 10      ",
"1       2       3       4       5       6       7PrnPar 8       9       10      ",
"X      XX      XX      XX      XX      XX      XX      XX      XX      XXX      "
};

void    main (void) {
    MENU      * mainmnu, * pmnu;
    IDEDT     * pidedt;
    register    i;
    bool        done = NO;


    if (!dbinidb (_PREVIOS, _PREVIOS)) return;

    for (i = 0; i < 3; i++) dbsl2nam (Db_ext [i] + 1, 1, NULL);
    mainmnu = mncrestd (itm, 9, MN_HOR, 2, 0, &mnubrd, &Att_Mnu2);
    wndsplay (Db_pstat, WN_NATIVE, WN_NATIVE);
    mndsplay (mainmnu, -1, -1);
    while (!done) {
        dbstatdb ();
        i = mnreadop (mainmnu, MN_NOREM);
        if (Cur_Doc == NULL && i != 0 && i != 8) {
            mnwarnin (NULL, 0, 1, "Активного документа не существует");
            continue;
        }
        switch (i) {
            case 0:
                * docname = EOS;
                if (dbrolnam (docname, Db_ext [DB_DOC] + 1, NULL, curname) == 1)
                    if (Cur_Doc == NULL || strcmp (curname, olddoc) != 0) {
                        dbdstroy (Cur_Doc);
                        dbdstroy (Cur_Qry);
                        dbdstroy (Cur_Lst);
                        Cur_Qry = Cur_Lst = NULL;
                        if ((Cur_Doc =
                                    dbinidoc (docname,DB_VISI,DB_DOC)) == NULL)
                            goto ret;
                        utmovabs (olddoc, curname, MAX_NAM_LEN, 0);
                    }
                break;
            case 1:
                if (Kb_sym == Down) break;
                dbfndrec ();
                break;
            case 2:
                if (Kb_sym == Down) break;
                if (!wndsplay (Cur_Doc->pwin, WN_NATIVE, WN_NATIVE)) goto ret;
                pidedt = utalloc (IDEDT);
                pidedt->funkey   = dockey;
                pidedt->prewait  = dbedstat;
                pidedt->postwait = edt_doc;
                pidedt->row      =  0;
                pidedt->col      = 21;
                dbdocedt (Cur_Doc, pidedt, YES, YES);
                free (pidedt);
                wnremove (Cur_Doc->pwin);
                break;
            case 3:
                if (Kb_sym == Down) break;
                if (!wndsplay (Cur_Doc->pwin, WN_NATIVE, WN_NATIVE)) goto ret;
                pidedt = utalloc (IDEDT);
                pidedt->funkey   = viewkey;
                pidedt->prewait  = dbedstat;
                pidedt->postwait = edt_doc;
                pidedt->row      =  0;
                pidedt->col      = 21;
                dbdocedt (Cur_Doc, pidedt, NO, YES);
                free (pidedt);
                wnremove (Cur_Doc->pwin);
                break;
            case 4:
                pmnu = mncrestd (savitm, 2, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
                mndsplay (pmnu, WN_CURMNU, WN_CURMNU);
                switch (mnreadop (pmnu, 0)) {
                    case    0:  dbed_svr (Cur_Doc, NO); break;
                    case    1:  dbed_2fl (Cur_Doc, NO); break;
                    default:                            break;
                }
                mndstroy (pmnu);
                break;
            case 5:   dbed_dlr (Cur_Doc);    break;
            case 6:   dbed_cld (Cur_Doc);    break;
            case 7:
                if (Kb_sym == Down) break;
                dbmklst ();
                break;
            case 8:
                if (Kb_sym != Down) done = YES;
                break;
            default : break;
        }
    }
ret:
    dbdstroy (Cur_Doc);
    dbdstroy (Cur_Lst);
    dbdstroy (Cur_Qry);
    mndstroy (mainmnu);
    utremove ("sld.ini");
    utremove ("slq.ini");
    utremove ("sll.ini");
    dbfindb  ();
    return;
}

bool    vfyfl2base (int handini) {
    int     handfile;
    DCB     dcb;
    bool    retcode = NO;
    byte  * p = Db_pwrk + 100;

    utread (handini, p, 13);
    if ((handfile = utopen (p, UT_R)) == -1) return NO;
    utread (handfile, &dcb, sizeof (dcb));
    if (strncmp (dcb.name_data, Cur_Doc->name_data, 8) == 0) retcode = YES;
    utclose (handfile);
    return retcode;
}

long  dbfndrec (void) {
    IDEDT  * pidedt;
    bool     success;
    int      hand;

    * Db_pwrk = EOS;
    if (dbrolnam ((char *)Db_pwrk,Db_ext [DB_QRY] + 1,vfyfl2base, curname) < 1)
        goto ret;
    if (Cur_Qry == NULL || strcmp (curname, oldqry) != 0) {
        dbdstroy (Cur_Qry);
        if ((Cur_Qry =
            dbinidoc ((char *) Db_pwrk, DB_VISI, DB_QRY)) == NULL) goto ret;
        utmovabs (oldqry, curname, MAX_NAM_LEN, 0);
    }

    if (!wndsplay (Cur_Qry->pwin, WN_NATIVE, WN_NATIVE)) goto ret;
    pidedt = utalloc (IDEDT);
    pidedt->funkey   = qrykey;
    pidedt->prewait  = dbedstat;
    pidedt->postwait = edt_query;
    pidedt->row      =  0;
    pidedt->col      = 21;
    success = dbdocedt (Cur_Qry, pidedt, YES, YES);
    free (pidedt);
    wnremove (Cur_Qry->pwin);
    if (!success) goto ret;
    hand = utcreate ("lsttmp");

    success = dbquery (hand);

ret:
    if (!success) utalarm ();
    utclose (hand);
    return 0L;  /* Раньше стоял numrec */
}

bool  dbmklst (void) {
    IDEDT     * pidedt;
    bool        retcode = YES;
    int         hand;

    * Db_pwrk = EOS;
    if (dbrolnam ((char *) Db_pwrk, Db_ext [DB_LST] + 1, vfyfl2base, NULL) < 1)
        return NO;
    if ((Cur_Lst = dbinidoc ((char *) Db_pwrk, DB_VISI, DB_LST)) == NULL)
        { retcode = NO; goto ret; }

    hand  = utopen ("lsttmp", UT_R);

    if (!dblistin (hand)) { retcode = NO; goto ret; }

    if (!wndsplay (Cur_Lst->pwin,WN_NATIVE,WN_NATIVE)) { retcode=NO; goto ret; }
    pidedt = utalloc (IDEDT);
    pidedt->funkey   = qrykey;
    pidedt->prewait  = dbedstat;
    pidedt->postwait = edt_doc;
    pidedt->row      =  0;
    pidedt->col      = 21;
    if (!dbdocedt (Cur_Lst, pidedt, YES, YES)) retcode = NO;
    free (pidedt);
    wnremove (Cur_Lst->pwin);
ret:
    utclose (hand);
    dbdstroy (Cur_Lst);
    Cur_Lst = NULL;
    return retcode;
}

