#include <sldbas.h>

static BORDER mnbord = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };
static ITEM   recitm [] = {
    { " Модификация ", "МмVv", NULL },
    { " Добавление  ", "ДдLl", NULL }
};

static ITEM   noyes[] = {
        { " Нет ", "НнYy",  NULL },
        { " Да ",  "ДдLl",  NULL }
};

void dbed_svr (DOC * doc, bool visi) {
    MENU    * pmnu;
    register  i;

    Cur_Edt = doc;
    if (visi && !dbed_c2v ()) return;
    if (doc->numrec > 0) {
        pmnu = mncrestd (recitm, 2, MN_VERT, 0, 0, &mnbord,
                                            (visi) ? &Att_Mnu1 : &Att_Mnu2);
        i = mnreadop (pmnu, 0);
        mndstroy (pmnu);
        if (i < 0) return;
        if (i == 1) doc->numrec = 0;
    }
    if (!dbsavrec (doc, doc->crtroot)) { utalarm (); Db_Err = NO_MEM; }
    if (visi) setnewfld ();
    return;
}

void dbed_ldr (bool nxt) {
    DOC     * doc = Cur_Edt;

    if (!dblodone (doc, doc->numrec + ((nxt) ? 1 : -1), nxt))
        { utalarm (); Db_Err = NO_MEM; }
    else doc->opt.change = NO;
    return;
}

void dbed_cld (DOC * doc) {

    if (!dbclrdoc (doc, YES)) { utalarm (); Db_Err = NO_MEM; }
    else doc->opt.change = NO;
    doc->numrec = 0L;
    return;
}

void dbed_dlr (DOC * doc) {

    if (mnwarnin (noyes, 2, 5, " ", "Подтвердите,", " ",
                                 "удалять текущую запись?", " ") < 1) return;
    if (!dbdelone (doc))  { utalarm (); Db_Err = NO_MEM; }
    if (!dblodone (doc, doc->numrec + 1,YES) &&
                                         !dblodone (doc, doc->numrec - 1, NO))
        dbed_cld (doc);
    doc->opt.change = NO;
    return;
}

