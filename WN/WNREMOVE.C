/***************************************************************************
    wnremove - ??????? ???? ? ??࠭?

bool wnremove (WINDOW *pwindow);

BWINDIW *pwindow    ?????⥫? ?? ????????? ???? WINDOW

??室:   YES        ???? 㤠????
         NO         ?訡??
***************************************************************************/

#include "language.h"
#include <slwind.h>

bool wnremove (WINDOW *pwindow) {
	bool ret = YES;

    if (!pwindow->options.shown) return (NO);
    if (Win_List == pwindow)     wnsizcur (0,0,0,0); /* cursor off */
    ret = wnhidewn (pwindow);
    wnpagrem (pwindow);
    if (ret == NO)               return (ret);
    pwindow->options.shown = 0;
    wnselect (Cur_Wind);
    return (YES);
}

