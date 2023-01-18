#include <slwind.h>
/****************************************************************************
 * cur   - может иметь значения от 0 до (total - 1);
 * total - общее кол-во
 * *************************************************************************/

void wnscrbar (WINDOW * pwin, bool right, long cur, long total) {
    int           row, col, bar;
    register      i, H = pwin->img.dim.h;
    char far    * pbuf, * pscreen, pfrom [] = { '', '', '', '░' };

    if (pwin->bord.type == BORD_NO_BORDER) return;

    col = pwin->where_shown.col;
    if (right) col += pwin->img.dim.w;
    else       col--;

    row = pwin->where_shown.row;
    total--;
    if (total <= 0L) bar = H - 1;
    else             bar = (int) ((long) (cur * H) / total);
    if (bar >= H)    bar = H - 1;

    for (i = 0; i < H; i++) {
        if      (i == bar)   pbuf = pfrom;
        else if (i == 0)     pbuf = pfrom + 1;
        else if (i == H - 1) pbuf = pfrom + 2;
        else                 pbuf = pfrom + 3;
        pscreen = wnviptrl (row + i, col);
        wnvicopy (&pbuf, &pscreen, 1, 1, NUM_COLS * 2, 0, Cmd[0]);
    }
    return;
}

