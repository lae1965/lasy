/*************************************************************************
    wncurmov - Move the current window's cursor.

void wncurmov (int row, int col);

int row;    Row relative to upper corner of window of new cursor position
int col;    Column relative to left corner of window of new cursor position

Description The current window's cursor is moved.  No change is
            visible unless the window has been selected via WNCURSOR
            to have the active cursor on its display page.
            An error occurs if the position is beyond the bounds of
            the window.

****************************************************************************/

#include <slwind.h>

void wncurmov (int row, int col) {

    Cur_Wind->cur_loc.row = (byte)row;
    Cur_Wind->cur_loc.col = (byte)col;
    if (!Cur_Wind->options.delayed)
        wnsetcur (Cur_Wind->where_shown.row + row,
                  Cur_Wind->where_shown.col + col);
    return;
}
