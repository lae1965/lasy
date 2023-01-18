#include <stdarg.h>
#include <slmenu.h>

void mnupdmnu (ATTRIB *patt, int num, ...) {
    MENU      * pmenu;
    MNITEM    * p;
    va_list     arg;
    int         i;
    char far  * pbuf, * pscreen;

    va_start (arg, num);
    for (i = 0; i < num; i++) {
        pmenu = va_arg (arg, MENU *);
        if (mnvalmnu (pmenu)) {
            p = pmenu->pitems;
            pmenu->pwin->attr = *patt;
            do  mnitmatr (pmenu, p);
            while ((p = p->next) != pmenu->pitems);
            pmenu->pwin->options.dirty = 1;
            if (pmenu->pwin->options.shown) {
                pbuf = (char far *) pmenu->pwin->img.pdata;
                pscreen = wnviptrl (pmenu->pwin->where_shown.row,
                                               pmenu->pwin->where_shown.col);
                wnvicopy (&pbuf, &pscreen, pmenu->pwin->img.dim.h,
                               pmenu->pwin->img.dim.w, NUM_COLS*2, 0, Cmd[1]);
                wnputbor (pmenu->pwin);
                pmenu->pwin->options.dirty = 0;
            }
        }
    }
    va_end (arg);
    return;
}

