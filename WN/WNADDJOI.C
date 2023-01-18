#include <slwind.h>

void wnaddjoi (WINDOW *pwin, int row, int col, int sym) {
    JOINTS *p;

    p = utalloc (JOINTS);   /*  */
    p->row = (byte)row;
    p->col = (byte)col;
    p->jtype = (char)sym;
    p->next = pwin->bord.pjoints;
    pwin->bord.pjoints = p;
    if (pwin->options.shown) wnputbor (pwin);
    return;
}

