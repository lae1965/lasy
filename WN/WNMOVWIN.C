#include <slwind.h>
#include <slkeyb.h>
#include <slsymb.def>


bool wnmovwin (WINDOW *pwin, int rowu, int coll, int rowd, int colr) {
    bool ret = NO, don = NO, we_shown = NO;
    int  hor, ver;
    int  row_sav, col_sav, row, col;
    int  wid     = pwin->img.dim.w;
    int  hei     = pwin->img.dim.h;
    int  bor     = (pwin->bord.type & BORD_TYPE)   ? 1 : 0;
    int  sha     = (pwin->bord.type & BORD_SHADOW) ? 1 : 0;
    int  row_min =  (rowu == WN_NATIVE) ? bor : bor + rowu;
    int  col_min =  (coll == WN_NATIVE) ? bor : bor + coll;
    int  row_max = ((rowd == WN_NATIVE) ? NUM_ROWS : rowd) - hei - bor - sha;
    int  col_max = ((colr == WN_NATIVE) ? NUM_COLS : colr) - wid - bor - sha*2;

    if (!pwin->options.shown) we_shown = wndsplay (pwin, WN_NATIVE, WN_NATIVE);
    row = row_sav = pwin->where_shown.row;
    col = col_sav = pwin->where_shown.col;
    while (!don) {
        hor = ver = 0;
        switch (kbgetkey (NULL)) {
            case GrayEnter:
            case Enter:  don = ret = YES;                       break;
            case Esc:    don = YES;                             break;
            case Home:   hor = -1;  ver = -1;                   break;
            case End:    hor = -1;  ver = +1;                   break;
            case PgUp:   hor = +1;  ver = -1;                   break;
            case PgDn:   hor = +1;  ver = +1;                   break;
            case Left:   hor = -1;                              break;
            case Right:  hor = +1;                              break;
            case Up:     ver = -1;                              break;
            case Down:   ver = +1;                              break;
            default :                                           break;
        }
        if (ver < 0)
            if (row > row_min) row--;
            else               ver = 0;
        if (ver > 0)
            if (row < row_max) row++;
            else               ver = 0;
        if (hor < 0)
            if (col > col_min) col--;
            else               hor = 0;
        if (hor > 0)
            if (col < col_max) col++;
            else               hor = 0;
        wnresize (pwin, ver, hor, ver, hor);
    }
    if (!ret) {
        wnremove (pwin);
        wndsplay (pwin, row_sav, col_sav);
    }
    if (we_shown)  wnremove (pwin);
    return (ret);
}

