/*************************************************************************
    wnnupblk - Mark a window as "dirty," and do update on a section if possible

bool wnnupblk (WINDOW *pwin, int r1, int c1, int r2, int c2);

WINDOW *pwin;       Pointer to WINDOW structure to update.
int     r1, c1;     Row and column of upper-left corner
                    of block to attempt to update.
int     r2, c2;     Row and column of lower-right corner
                    of block to attempt to update.

Description This function writes any pending I/O requests which
            are in the rectangle bounded by (r1,c1)-(r2,c2) to the
            designated window if there are any waiting, and the
            window is not delayed, not frozen, etc.

Returns     YES         Ok
            NO          Can't update window
*****************************************************************************/

#include <slwind.h>

bool wnupdblk (WINDOW *pwin, int r1, int c1, int hei, int wid) {
    CELLSC      *pfrom;
    char far    *pbuf, *pscreen;
    int          W, WROW, WCOL_;

    if (pwin->options.delayed || !pwin->options.shown || pwin->options.covered)
        return NO;

    W = pwin->img.dim.w;
    WROW = pwin->where_shown.row;
    WCOL_ = pwin->where_shown.col + c1;

    pfrom = pwin->img.pdata + ((r1 * W) + c1);
    for (; hei > 0; r1++, hei--,  pfrom += W) {
        pbuf    = (char far *) pfrom;
        pscreen = wnviptrl (WROW + r1, WCOL_);
        wnvicopy (&pbuf, &pscreen, 1, wid, NUM_COLS * 2, 0, Cmd[1]);
    }
    return YES;
}

