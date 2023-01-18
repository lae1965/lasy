/***************************************************************
    wnupdate - Write pending I/O requests to a window

bool wnupdate (WINDOW *pwin);

WINDOW *pwin;       Pointer to WINDOW structure to update.

Description This function writes any pending I/O requests to the
            designated window if there are any waiting.  (Nothing
            will occur if any portion of the window's data area is
            covered by another window.)

            Also updates cursor.

            This function does not designate the window "not
            delayed".  Use WNSETOPT to designate a window "delayed"
            or "not delayed".

            An error occurs if pwin does not point to a valid window
            structure or if the window is not currently displayed.

Returns     YES         Ok
            NO          Error
*******************************************************************/
#include <slwind.h>

bool wnupdate (WINDOW *pwin) {
    char far   * pbuf, * pscreen;
    register   Wrow = pwin->where_shown.row, Wcol = pwin->where_shown.col;

    if (!pwin->options.shown || !pwin->options.dirty || pwin->options.covered)
        return (YES);
    pbuf = (char far *) pwin->img.pdata;
    pscreen = wnviptrl (Wrow, Wcol);
    wnvicopy (&pbuf, &pscreen, pwin->img.dim.h, pwin->img.dim.w, NUM_COLS*2,
                                                                    0, Cmd[1]);
    pwin->options.dirty = 0;
    wnsetcur (Wrow + pwin->cur_loc.row, Wcol + pwin->cur_loc.col);
    wnsizcur (pwin->options.cur_on, pwin->cur_typ.high,
                                                pwin->cur_typ.low, WN_CUR_ADJ);
    return YES;
}

