/************************************************************************
    wnscrblk - Scroll the memory copy of a window.

bool wnscrblk (WINDOW *pwin, int r1, int c1, int hei, int wid,
                                    int attr, int dir, int off, int option);

WINDOW *pwin    Pointer to the window to scroll in.
int r1, c1      Upper-left corner of region to scroll
int hei, wid    Height and width of block to scroll
int attr        Attribute of new blank lines.
                (WN_NATIVE specifies window's native color).
int dir         Scrolling direction
                WN_SCR_UP
                WN_SCR_DOWN
                WN_SCR_LEFT
                WN_SCR_RIGHT
int off         Number of rows (or columns) to scroll.
                A value of 0 specifies that all
                characters lines within the region should be cleared.
int option      WN_UPDATE
                WN_NO_UPDATE

Description This function scrolls rows/columns of characters (with
            their attributes) up or down, left or right within the
            specified subsection of a window.
            The vacant rows are filled with blanks and a specified
            attribute.

Returns     YES         Ok
            NO          Can't update
**************************************************************************/

#include <slwind.h>


bool wnscrblk (WINDOW *pwin, int r1, int c1, int hei, int wid,
                                  int attr, int dir, int off, int option) {
    CELLSC         *pto, *pfrom;
    register        r, i = 1;
    int             r1_ = r1, c1_ = c1, hei_ = hei, wid_ = wid;
    int             W = pwin->img.dim.w;
    word            count;

    if (attr == WN_NATIVE)  attr = pwin->attr.text;

    if (off != 0)
        switch (dir) {
            case WN_SCR_UP:
                count   = wid * sizeof (CELLSC);
                pto   = pwin->img.pdata + (W * r1) + c1;
                pfrom = pto + (W * off);

                for (r = hei - off; r > 0; r--,  pto += W,  pfrom += W)
                    utmovmem ((char far *) pto, (char far *) pfrom, count);

                r1_ = r1 + (hei - off);
                hei_ = off;
                break;

            case WN_SCR_DOWN:
                count   = wid * sizeof (CELLSC);
                pto   = pwin->img.pdata + (W * (r1 + hei - 1)) + c1;
                pfrom = pto - (W * off);

                for (r = hei - off; r > 0; r--,  pto -= W,  pfrom -= W)
                    utmovmem ((char far *) pto, (char far *) pfrom, count);

                hei_ = off;
                break;

            case WN_SCR_LEFT:
                count   = (wid - off) * sizeof (CELLSC);
                pto   = pwin->img.pdata + (W * r1) + c1;
                pfrom = pto + off;

                for (r = hei; r > 0; r--,  pto += W,  pfrom += W)
                    utmovmem ((char far *) pto, (char far *) pfrom, count);

                c1_ = c1 + wid - off;
                wid_ = off;
                break;

            case WN_SCR_RIGHT:
                count   = (wid - off) * sizeof (CELLSC);
                pfrom   = pwin->img.pdata + (W * r1) + c1;
                pto     = pfrom + off;

                for (r = hei; r > 0; r--,  pto += W,  pfrom += W)
                    utmovmem ((char far *) pto, (char far *) pfrom, count);

                wid_ = off;
                break;
        }
    pto = pwin->img.pdata + (W * r1_) + c1_;

    for (r = hei_; r > 0; r--, pto += W) wnmovpca (pto, ' ', wid_,(byte)attr);

    if (option == WN_UPDATE) i = !wnupdblk (pwin, r1, c1, hei, wid);
    pwin->options.dirty = i;
    return (i);
}

