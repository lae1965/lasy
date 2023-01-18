#include    <sldbas.h>
#include    <process.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <slmous.h>
#include    <slmenu.h>
#include    <stdlib.h>
#include    <alloc.h>
#include    <dos.h>
#include    <dir.h>
#include    <io.h>

static char  phelpname [80];

static void     help            (void);
static int      ed_help         (void);

void    main (void) {
    char      * pfilnam;
    register    hand;
    long        hz;

    hz = coreleft ();

    if (!dbinidb (_TEXTC80, _MINROWS, NULL)) return;

    dbsl2nam (Db_ext [0] + 1, 1, NULL);

    if ((pfilnam = dbfilnam ("HELP", Db_ext [DB_DOC]+1))==NULL) goto ret;
    if ((Cur_Doc = dbinidoc (pfilnam, DB_VISI|DB_MAXWIN, DB_DOC, NO)) == NULL)
        goto ret;

    if (mnfnminp (WN_CENTER, WN_CENTER, "Введите имя Help'a",
                                            phelpname, ".hlp", YES, NO)) {
        hand = utopen (phelpname, UT_R);
        dbloddoc (Cur_Doc, hand);
        utclose  (hand);

    }
    help ();
ret:
    utremove ("sld.ini");
    dbdstroy (Cur_Doc);

    dbfindb ();

    hz -= coreleft ();
    if (hz != 0L) utalarm ();

    return;
}

static void help (void) {
    DOC       * doc = Cur_Doc;
    IDEDT     * pidedt;

    dbdsplay (doc, WN_NATIVE, WN_NATIVE, NULL);
    pidedt = utalloc (IDEDT);
    pidedt->funkey   = NULL;
    pidedt->prewait  = dbedstat;
    pidedt->postwait = ed_help;
    pidedt->row      =  0;
    pidedt->col      = 21;
    dbdocedt (doc, pidedt, YES, YES);
    free (pidedt);
    return;
}

static int ed_help (void) {
    int     hand;


    switch (Kb_sym) {
        case F7:               dbprprn (dbpreview);                   return 1;
        case ShiftF5:          dbed_2fl (Cur_Edt, YES);       return 0;
        case F2:      dbed_sde ();          break;
        case F3:      dbed_par ();          break;
        case ShiftF1: dbed_clb ();          break;
        case ShiftF2: dbed_mov ();          break;
        case ShiftF4: dbed_jst ();          break;
        case Esc:
        case AltX:
            if (!Cur_Doc->opt.change) return 2;
            while (!wnstrinp (WN_CENTER, WN_CENTER, " Имя Help'а ",
                                                              phelpname, 70));


            hand = utcreate (phelpname);
            dbsavdoc (Cur_Edt, Cur_Edt->crtroot, hand);
            utclose (hand);
            return  2;
        default :              return -2;
    }
    return 0;
}

