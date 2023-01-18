/**************************************************************************
    wnhidewn - Conceal window but leave it attached to linked window list

bool wnhidewn (WINDOW *pwin);

WINDOW *pwin;   Pointer to WINDOW structure to conceal.

Description This function makes a currently-displayed window
            invisible and restores the previous contents of the
            screen.  The window's location and border remain set so
            the window may later be redisplayed via WNUNHIDE.

Returns     YES         Ok
            NO          Error
***********************************************************************/

#include "language.h"
#include <slwind.h>

                /* Hide covering windows.                             */
static WINDOW  *undisp (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim);
                /* Mark lower windows uncovered.                      */
static WINDOW  *uncovr (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim);
                /* Check for covering windows.                        */
static WINDOW  *upcovr (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim);
                /* Redisplay covering windows.                        */
static WINDOW  *redisp (WINDOW *pnode);

bool wnhidewn (WINDOW *pwin) {
    bool      result = YES;
    int       row, col, high, low;
    WINDOW   *pnode;

    if (!pwin->options.shown) return (YES);
    if (!pwin->options.removable) return (NO);
    if (undisp (pwin,&pwin->where_prev,&pwin->prev.dim) == NULL)  result = NO;
    pwin->options.temp_hid  = 0;
    pwin->options.shown     = 0;
    if (uncovr (pwin, &pwin->where_prev, &pwin->prev.dim) == NULL) result = NO;
    if (Cur_Wind == pwin) {
        Cur_Wind = NULL;
        wngetcur (&row, &col, &high, &low);
        wnsizcur (0, high, low, WN_CUR_NAD);
    }
    if (redisp(pwin) == NULL) result = NO;
    for (pnode = Win_List; pnode != NULL; pnode = pnode->below) {
        if (!pnode->options.delayed)
            if (!wnupdate (pnode)) { result = NO; break; }
    }
    return (result);
}

static WINDOW *undisp (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim) {
    WINDOW      * presult = pnode;
    WINDOW      * pwin    = pnode;
    char far    * pbuf, * pscreen;

    if (pnode->above != NULL)
        if (undisp (pnode->above, pcorner, pdim) == NULL)  presult = NULL;

    if ((pwin->options.shown) && (!pwin->options.temp_hid) &&
         wnovrlap (pcorner,pdim,&pwin->where_prev,&pwin->prev.dim)) {
        if (pnode->above != NULL)
            if (undisp(pnode->above,&pwin->where_prev,&pwin->prev.dim) == NULL)
                presult = NULL;

        if (pwin->options.removable) {
            pbuf    = (char far *) pwin->prev.pdata;
            pscreen = wnviptrl (pwin->where_prev.row, pwin->where_prev.col);
            wnvicopy (&pbuf, &pscreen, pwin->prev.dim.h, pwin->prev.dim.w,
                                                     NUM_COLS*2, 0, Cmd[1]);
        }
        pwin->options.temp_hid = 1;
        pwin->options.dirty    = 0;
    }
    return (presult);
}

static WINDOW   *uncovr (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim) {
    WINDOW   *presult;
    WINDOW   *pwin = pnode;

    if (wnovrlap (pcorner, pdim, &pwin->where_prev, &pwin->prev.dim)) {
        if (pnode->above == NULL ||
           upcovr (pnode->above, &pwin->where_prev, &pwin->prev.dim) == NULL)
              pwin->options.covered = 0;
    }
    presult = pnode;
    if (pnode->below != NULL)
        if (uncovr (pnode->below, pcorner, pdim) == NULL) presult = NULL;
    return (presult);
}

static WINDOW *upcovr (WINDOW *pnode, LOCATE *pcorner, DIMREC *pdim) {
	WINDOW *presult;

    if (pnode->options.shown &&
        wnovrlap (pcorner, pdim, &pnode->where_prev, &pnode->prev.dim))
            presult = pnode;

    else if (pnode->above != NULL)
        presult = upcovr (pnode->above, pcorner, pdim);
    else  presult = NULL;
	return (presult);
}

static WINDOW   *redisp (WINDOW *pnode) {
    WINDOW *pwin = pnode;
    char far    * pbuf, *pscreen;

    if ((pwin->options.shown) && (pwin->options.temp_hid)) {
        if (pwin->options.removable) {
            pwin->prev.pdata = (CELLSC *)
                calloc (pwin->prev.dim.h * pwin->prev.dim.w, sizeof (CELLSC));
            pbuf = (char far *) pwin->prev.pdata;
            pscreen = wnviptrl (pwin->where_prev.row, pwin->where_prev.col);
            wnvicopy (&pscreen, &pbuf, pwin->prev.dim.h, pwin->prev.dim.w,
                                                     NUM_COLS*2, 0, Cmd[6]);
        }
        wnputbor (pwin);
        pbuf    = (char far *) pwin->img.pdata;
        pscreen = wnviptrl (pwin->where_shown.row, pwin->where_shown.col);
        wnvicopy (&pbuf,&pscreen, pwin->img.dim.h, pwin->img.dim.w,
                                                    NUM_COLS*2, 0, Cmd[1]);
        pwin->options.dirty    = 0;
        pwin->options.temp_hid = 0;
        wncoverw (pnode->below);
    } else if (pwin->options.temp_hid) pwin->options.shown = 0;

    if (pnode->above != NULL && redisp (pnode->above) == NULL) return (NULL);
    return (pnode);
}

