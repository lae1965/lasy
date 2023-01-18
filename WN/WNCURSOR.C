/**************************************************************************
    wncursor - Activate a window's cursor

bool wncursor (WINDOW *pwindow);

WINDOW *pwindow;    Pointer to WINDOW structure to select.

Description The window's cursor is activated (although the cursor
            may be invisible).  The cursors of other windows on that
            page are deactivated.

            An error occurs if pwindow does not point to a valid
            window structure, if the window is hidden or not
            currently displayed, or if some portion of the window's
            data area is covered by another window.

Returns     YES         Ok
            NO          Error
*************************************************************************/

#include <slwind.h>

bool wncursor (WINDOW *pwindow) {

    if (!pwindow->options.shown || pwindow->options.covered ||
        pwindow->options.delayed || pwindow->options.dirty)  return (NO);

    Cur_Wind = pwindow;

    wnsetcur (pwindow->where_shown.row + pwindow->cur_loc.row,
              pwindow->where_shown.col + pwindow->cur_loc.col);
    wnsizcur (pwindow->options.cur_on, pwindow->cur_typ.high,
              pwindow->cur_typ.low, WN_CUR_ADJ);
    return (YES);
}

