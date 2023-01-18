/*************************************************************************
    wncoverw - Mark lower windows if they are covered by a given window.

wncoverw (WINDOW *pwind);

WINDOW   *pwind;    Pointer to uppermost window to examine.

Returns     YES         Ok
            NO          Error
*************************************************************************/

#include <slwind.h>

#define high    (int)pwin->cur_typ.high
#define low     (int)pwin->cur_typ.low

void   wncoverw (WINDOW *pwind) {
    WINDOW   *pwin = pwind->below;

	if (pwin == NULL || !pwind->options.shown) return;
    do {
        if (pwin->options.shown &&
            wnovrlap (&(pwind->where_prev), &(pwind->prev.dim),
                      &(pwin->where_prev),  &(pwin->prev.dim))) {
            pwin->options.covered = 1;
            if (pwin == Cur_Wind) {
                wnsizcur (NO, high, low, WN_CUR_NAD);
                Cur_Wind = NULL;
            }
        }
    } while ((pwin = pwin->below) != NULL);
}

