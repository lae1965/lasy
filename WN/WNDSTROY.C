/****************************************************************************
    wndstroy - ??????? ???? ? ??࠭?, ࠧ??蠥? ?????????

void wndstroy (WINDOW *pwin);

WINDOW  *pwin       ?????⥫? ?? ????????? ???? WINDOW
*****************************************************************************/

#include <slwind.h>

void wndstroy (WINDOW *pwin) {

    if (!wnvalwin (pwin))  return;
    if (pwin->options.shown) wnremove (pwin);
    utfree (&pwin->prev.pdata);
    utfree (&pwin->img.pdata);
    wnundjoi (pwin);
    pwin->sign[0] = EOS;
    if (pwin == Cur_Wind)  Cur_Wind = NULL;
    free ((char *) pwin);
    return;
}

