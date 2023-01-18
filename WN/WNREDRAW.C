/************************************************************************
    wnredraw - Redisplay all windows currently shown

bool wnredraw (void);

Description This function redisplays all windows currently shown on
            a video display page as they are currently recorded in
            internal data structures.  If one of the displayed
            windows is designated to have an active cursor, then its
            cursor is restored.  Hidden windows retain their borders
            and positions but are not displayed.

Returns     YES         Ok
            NO          Error
**************************************************************************/

#include <slwind.h>

static void redraw (WINDOW *);

bool wnredraw (void) {
    bool    result = YES;
    WINDOW *pnode;

    if ((pnode = Win_List) != NULL) {
        redraw (pnode);
        if ((pnode = Cur_Wind) != NULL && !wncursor (pnode))  result = NO;
    }
    return (result);
}

static void redraw (WINDOW *pnode) {
    WINDOW     * pwin = pnode;
    char far   * pbuf, * pscreen;

    pwin->options.temp_hid = 0;
    if (pnode->below != NULL) redraw (pnode->below);
    if (pwin->options.shown) {
        wnputbor (pwin);
        pbuf = (char far *) pwin->img.pdata;
        pscreen = wnviptrl (pwin->where_shown.row, pwin->where_shown.col);
        wnvicopy (&pbuf, &pscreen, pwin->img.dim.h, pwin->img.dim.w,
                                                    NUM_COLS*2, 0, Cmd[1]);
    }
    pwin->options.dirty = 0;
    return;
}

