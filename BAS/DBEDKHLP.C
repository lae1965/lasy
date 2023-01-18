#include <sldbas.h>
#include <slkeyb.h>


void dbedkhlp (void) { /*** Выз.во время ожидания клавиши по смене статуса ***/
    char far    * pbuf, * pscreen;
    char        * caps = "";
    byte          batt = Att_Khlp.bold;
    CELLSC      * pdata = Db_phlp->img.pdata;
    int           i;

    if (Db_curidedt->funkey != NULL) {
        if (Db_curidedt->funkey [0] != NULL) {
            if      (Kb_sym & KB_SHIFTSTAT) i = 1;
            else if (Kb_sym & KB_CTRLSTAT)  i = 2;
            else if (Kb_sym & KB_ALTSTAT)   i = 3;
            else                            i = 0;
        } else i = 3;
        i *= 2;
        pbuf = Db_curidedt->funkey [i];
        if (pbuf == NULL) goto clr;
        wnmovps  (pdata, pbuf, 80);
        dbkhlpat (pdata, Db_curidedt->funkey [i + 1], Att_Khlp.text, 
                                                                Att_Khlp.bold);
    } else
clr:
        wnmovpca (pdata, ' ', 80, batt);

    pbuf = (char far *) pdata;
    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS * 2, 0, Cmd[1]);
    if (Db_curidedt->col == -1) return;

    if (Kb_status.caps_state)   pbuf = (char far *) caps;
    else                        pbuf = (char far *) (caps + 1);

    pscreen = wnviptrl ((int) Db_curidedt->row, (int) Db_curidedt->col);
    wnvicopy (&pbuf, &pscreen, 1, 1, NUM_COLS*2, Att_Khlp.afld, Cmd[2]);
    return;
}

