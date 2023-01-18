#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <alloc.h>

static int      edt_prn     (void);

static char * funkey [] = { NULL, NULL, NULL, NULL, NULL, NULL,
"Enter или Левая кнопка мыши   Назначить / отменить новую страницу   Esc  Выход  ",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXX                                       XXXXX        "
};

void dbpreview (void) {
    DOC         * doc = Cur_Edt;
    IDEDT       * pidedt;
    bool          was_shown = doc->pwin->options.shown;

    doc->view.line = 0L;
    doc->view.row = doc->view.pos = 0;
    if (!was_shown) dbdsplay (doc, WN_NATIVE, WN_NATIVE, NULL);
    pidedt = utalloc (IDEDT);
    pidedt->funkey      = funkey;
    pidedt->prewait     = dbedstat;
    pidedt->aftergetkey = edt_prn;
    pidedt->row         =  0;
    pidedt->col         = 21;
    dbdocedt (doc, pidedt, NO, NO);
    free (pidedt);
    if (!was_shown) wnremove (doc->pwin);
    return;
}

static int edt_prn (void) {

    switch (Kb_sym) {
        case Right:
        case ShiftRight:     dbviewhor  (YES);  break;
        case Left:
        case ShiftLeft:      dbviewhor  (NO);   break;
        case Down:
        case ShiftDown:      dbviewver  (YES);  break;
        case Up:
        case ShiftUp:        dbviewver  (NO);   break;
        case SpecK1:
            mnmakebrk   (Cur_Edt->view.line - (Cur_Edt->view.row - Db_MousY));
            Cur_Edt->opt.needout = YES;
            break;
        case Enter:
            mnmakebrk   (Cur_Edt->view.line);
            Cur_Edt->opt.needout = YES;
            break;
        case Esc:
        case AltX:                              return 2;
        default :            utalarm ();        break;
    }
    return 0;
}

