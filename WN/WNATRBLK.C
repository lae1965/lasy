/**************************************************************************
    wnatrblk - Change attributes on block of a window.

void wnatrblk (WINDOW *pwin, int r1,int c1,int hei,int wid, int attr, int opt);

WINDOW *pwin;   Pointer to WINDOW structure to change.
int     r1, c1; Row and column of upper-left corner of block to set attributes
                in (relative to upper-left corner of window).
int     hei, wid; hei and wid of block to set attributes
int     attr;   New attribute
int     option; WN_UPDATE       to update the window after changing
                WN_NO_UPDATE    to not update - the attribute change will
                                be in memory only.

Description This function fills a rectangular block of the data area
            of a window with a given attribute without affecting
            the characters already displayed there.  The cursor
            is not moved.
            This function does not affect the window's default
            attributes.  Use WNSETOPT for that purpose.

*************************************************************************/

#include <slwind.h>

void wnatrblk (WINDOW *pwin,int r1,int c1,int hei,int wid, int attr, int opt) {
    register    r = r1, j = r1 + hei;
    int         W = pwin->img.dim.w;
    CELLSC    * pdata = pwin->img.pdata + r1 * W + c1;

    for (r = r1;  r < j;  r++, pdata += W)      wnmovpa (pdata, wid, attr);
    if (opt == WN_UPDATE)
        pwin->options.dirty = !wnupdblk (pwin, r1, c1, hei, wid);
    else pwin->options.dirty = 1;
    return;
}

