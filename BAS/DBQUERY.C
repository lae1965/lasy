/******************************************************************************/
/*   dbquery () - осуществляет поиск записей в базе по запросу на поиск.      */
/*                                                                            */
/*   int dbquery (handlst);                                                   */
/*                                                                            */
/*   int handlst    - handle файла, в который заносятся номера                */
/*                    записей в базе, удовлетворяющих запросу.                */
/*                                                                            */
/*   Возвращает:      Количество найденных записей;                           */
/*                    -1, если запрос издан невалидно                         */
/******************************************************************************/

#include   <sldbas.h>
#include   <slkeyb.h>


int dbquery (int handlst) {
    DOC    * basedoc, * qrydoc;
    IDBASE * pdb;
    char   * pfilnam;
    long     total;
    int      num = 0;

    basedoc = dbinidoc (Cur_Qry->name_data, 0, DB_BAS, Cur_Qry->opt._global);

    pfilnam = dbfilnam ((char *) Cur_Qry->cnam [0].nam, Db_ext [DB_QRY] + 1);
    qrydoc  = dbinidoc (pfilnam, 0, DB_QRYWRK, NO);
    if (!dbq2qry (Cur_Qry, qrydoc)) { num = -1; goto ret; }

    pdb = dbopendb (Cur_Qry->name_data, Cur_Qry->opt._global, UT_R);

    total = pdb->slhd.total;

    wndiswrk (total, 5, " ", " ", "           Идет поиск          ", " ", " ");

    basedoc->numrec = 0L;
    while (total > 0) {
        wnupdwrk (1L);
        if (kbvfysym (1)) break;     /* Esc */
        basedoc->numrec++;
        dblodrec (basedoc, pdb, basedoc->numrec, YES);
        if (dbtstrec (basedoc, qrydoc, basedoc->crtroot, qrydoc->crtroot)) {
            num++;
            utwrite (handlst, &basedoc->numrec, sizeof (long));
        }
        total--;
    }

    wnremwrk ();
    dbclosdb (pdb);

ret:
    dbclrfrg (qrydoc);
    dbdstroy (qrydoc);
    dbdstroy (basedoc);

    return num;
}

