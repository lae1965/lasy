/*************************************************************************
    wnunhide - Redisplay hidden window

bool wnunhide (WINDOW *pwin);

WINDOW *pwin;   Pointer to WINDOW structure to redisplay.

Description This function redisplays a window that has been
            previously displayed but is now hidden.  The window's
            location and border are the same as before.

Returns     YES         Ok
            NO          Error
************************************************************************/

#include <slwind.h>

bool wnunhide (WINDOW *pwin) {
    char far    * pbuf, * pscreen;

    if (pwin->options.shown)      return (YES);
    if (wnpagrem (pwin) == NO)    return (NO);
    if (wnpagadd (pwin) == NULL)  return (NO);
    if (pwin->options.removable) {
        pbuf = (char far *) pwin->prev.pdata;
        pscreen = wnviptrl (pwin->where_prev.row, pwin->where_prev.col);
        wnvicopy (&pscreen, &pbuf, pwin->prev.dim.h, pwin->prev.dim.w,
                                                    NUM_COLS*2, 0, Cmd[6]);
    }
    pbuf    = (char far *) pwin->img.pdata;
    pscreen = wnviptrl (pwin->where_shown.row, pwin->where_shown.col);
    wnvicopy (&pbuf,&pscreen, pwin->img.dim.h, pwin->img.dim.w,
                                                    NUM_COLS*2, 0, Cmd[1]);
    wnputbor (pwin);
    pwin->options.dirty         =   0;
    pwin->options.covered       =   0;
    pwin->options.shown         =   1;
    wncoverw (pwin);
    return (YES);
}

