#include    <sldbas.h>
#include    <alloc.h>

static  char pstat1 [] =
"Документ:                      LaSy 1.00 Найдено:            ";
static  char pstat2 [] =
"Текущая запись:                                                                 ";
/*
 01234567890123456789012345678901234567890123456789012345678901234567890123456789
		   1         2         3         4         5         6         7
*/

void dbstatdb (void) {
    DOC      * doc = Cur_Doc;
    CELLSC   * pdata = Db_pstat->img.pdata;
    char     * p;
    byte       nfld = Att_Khlp.nfld, afld = Att_Khlp.afld;
    register   i;

    wnmovpsa (pdata,      pstat1, 61, nfld);
    wnmovpsa (pdata + 80, pstat2, 80, nfld);
    pdata [63].at = afld;

    if (doc != NULL) {
        p = (char *) doc->cnam [0].nam;
        wnmovpsa  (pdata + 10, p, MAX_NAM_LEN, afld);

        i = utl2crdx (doc->numrec, Db_pwrk, 0, 10);
        wnmovpsa (pdata + 96, Db_pwrk, i, afld);
    }

    wnupdblk (Db_pstat, 0, 0, 2, NUM_COLS);
    return;
}

