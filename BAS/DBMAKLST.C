#include <sldbas.h>

/**************************************************************************
 * bool lstfile == YES формировать из списка найденных
 * bool page    == YES количество экземпляров в порции определяется из размеров
 *                     окна;
 *                 NO  порция загружается, пока хватает памяти.
 **************************************************************************/

bool dbmaklst (bool lstfile, bool page) {

    strcpy (idlst.name_data, Cur_Lst->name_data);
    idlst.db_global = Cur_Lst->opt._global;

    if (dbmakidlst (lstfile, page)) return (dblodlst (1));
    return NO;
}

bool dbmakidlst (bool lstfile, bool page) {
    IDBASE    * pdb;
    int         handlst;

    idlst.lstfile = lstfile;
    if (page) idlst.H = Cur_Lst->pwin->img.dim.h;
    else      idlst.H = -1;

    idlst.first = idlst.last = 0L;
    idlst.wasrecs = 0L; idlst.currecs = 0;

    if (lstfile) {
        handlst = utopen ("lst.tmp",  UT_R);
        idlst.total = utfsize (handlst) / sizeof (long);
        utclose (handlst);
    } else {
        pdb = dbopendb (idlst.name_data, idlst.db_global, UT_R);
        idlst.total = pdb->slhd.total;
        dbclosdb (pdb);
    }
    if (idlst.total == 0L) return NO;
    return YES;
}

