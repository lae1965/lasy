/*******************************************************************************
    Функция dbunirec сохраняет запись в базу, предварительно проверяя ее на
            уникальность.


    bool dbunirec (DOC * docfrom, char * basenam);

    DOC     * docfrom - документ, из которого формируется запись;
    char    * basenam - мнемоничное имя базы с уникальными записями.

    Возвращает:     YES - запись уникальная и сохранена
                     NO - наоборот
*******************************************************************************/

#include <sldbas.h>
#include <alloc.h>



bool dbunirec (DOC * docfrom, char * basenam, bool _global) {
    char    * pfilnam, * pbuf1, * pbuf2;
    DOC     * basedoc;
    IDBASE  * pidb;
    long      total, l;
    int       hand;
    word      reclen;
    bool      retcode = YES;

    pfilnam = dbfilnam (basenam, Db_ext [DB_BAS] + 1);
    basedoc = dbinidoc (pfilnam, 0, DB_BAS, _global);
    pidb    = dbopendb (pfilnam, _global, UT_RW);

    dbclrdone (docfrom->crtroot);
    dbcrtocr (basedoc, docfrom, basedoc->crtroot, docfrom->crtroot, NO);
    hand = utcreate ("rec.tmp");
    dbsavdoc (basedoc, basedoc->crtroot, hand);
    reclen = (word) utlseek (hand, 0L, UT_END);
    pbuf1 = malloc (reclen);
    pbuf2 = malloc (reclen);
    utlseek (hand, 0L, UT_BEG);
    utread (hand, pbuf1, reclen);
    utclose (hand);
    utremove ("rec.tmp");

    total = pidb->slhd.total;
    utlseek (pidb->handh, sizeof (SLHD), UT_BEG);

    while (total > 0L) {
        do utread (pidb->handh, &l, sizeof (long));
        while (l == -1L);
        utlseek (pidb->handb, l, UT_BEG);
        utread  (pidb->handb, &l, sizeof (long));
        if ((word) (l - sizeof (long)) == reclen) {
            utread (pidb->handb, pbuf2, reclen);
            if (utbufcmp (pbuf1, pbuf2, reclen) == 0)
                { retcode = NO; goto ret; }
        }
        total--;
    }
    dbclrdone (basedoc->crtroot);
    basedoc->numrec = 0L;
    dbsavtmp (basedoc, basedoc->crtroot, pidb);
ret:
    free (pbuf2);
    free (pbuf1);
    dbclosdb (pidb);
    dbdstroy (basedoc);

    return retcode;
}

