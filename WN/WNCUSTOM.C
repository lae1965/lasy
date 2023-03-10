/**************************************************************************
    wncustom - ?ਢ??뢠?? ???? ? ????? ?? ??࠭?

bool wncustom (WINDOW *pwin,  int row, int col);

WINDOW *pwin;   ?????⥫? ?? ????????? ????
int     row;    ??ப?  ?????? ???孥?? 㣫? ???? ????
                WN_NATIVE   ??? ?뫮 㪠???? ??? ᮧ?????
                WN_CENTER   ??????஢???? ?? ???⨪???
                WN_CURMNU   ?ਢ離? ? ⥪?饬? ????
                >= 0        ???? ???祭?? row
int     col;    ??????? ?????? ???孥?? 㣫? ???? ????
                WN_NATIVE   ??? ?뫮 㪠???? ??? ᮧ?????
                WN_CENTER   ??????஢???? ?? ???⨪???
                WN_CURMNU   ?ਢ離? ? ⥪?饬? ????
                >= 0        ???? ???祭?? col

??室:   YES    ???? ?ਢ易??
         NO     ?訡??

****************************************************************************/

#include <slmenu.h>


bool  wncustom (WINDOW *pwin, int row, int col) {
    int mode, i;
    int wrow  = pwin->where_shown.row;
    int wcol  = pwin->where_shown.col;
    int H     = pwin->img.dim.h;
    int W     = pwin->img.dim.w;

    if (row == WN_CURMNU) {
        if (Cur_Menu != NULL) {
            mode = ((pwin->bord.type & BORD_TYPE) == 0) ? 1 : 2;
            wrow = Cur_Menu->pwin->where_shown.row+Cur_Menu->pcur->row+mode;
        }  else   row = WN_CENTER;
    }
    if (row == WN_CENTER)   wrow = (NUM_ROWS - 1 - H) / 2;
    else if (row >= 0)      wrow = row;

    if (col == WN_CURMNU) {
        if (Cur_Menu != NULL) {
            mode = ((pwin->bord.type & BORD_TYPE) == 0) ? 0 : 1;
            wcol = Cur_Menu->pwin->where_shown.col + Cur_Menu->pcur->col + mode;
            if (Cur_Menu->pcur->vpos > 0)   wcol += Cur_Menu->pcur->vpos - 2;
        }
        else   col = WN_CENTER;
    }
    if (col == WN_CENTER)  wcol = (NUM_COLS - W) / 2;
    else if (col >= 0)     wcol = col;

    mode = ((pwin->bord.type & BORD_TYPE) == 0) ? 0 : 1;
    if (wrow == 0 && mode == 1)    wrow++;
    if (wcol == 0 && mode == 1)    wcol++;

    if (wrow > (i = NUM_ROWS - mode - H)) wrow = i;
    if (wcol > (i = NUM_COLS - mode - W)) wcol = i;

    if ((pwin->bord.type & BORD_SHADOW)) {
        row = wrow + H - (NUM_ROWS - mode - 1);
        if (row > 0 && wrow > row-1)  wrow -= row;
        col = wcol + W - (NUM_COLS - mode - 2);
        if (col > 0 && wcol > col-1)  wcol -= col;
    }
    if (wrow < 0 || wcol < 0) return (NO);
    pwin->where_shown.row = wrow;
    pwin->where_shown.col = wcol;
    return (YES);
}

