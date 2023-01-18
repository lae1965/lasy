/***************************************************************************
    wnundjoi - Undisplay border joints

void wnundjoi (WINDOW *pwin);

WINDOW *pwin       pointer to WINDOW structure

***************************************************************************/

#include <slwind.h>

void wnundjoi (WINDOW *pwin) {
    JOINTS *p, *q;

    if ((p = pwin->bord.pjoints) == NULL) return;

    do { q = p->next; free (p); p = q; }
    while (p != NULL);
    pwin->bord.pjoints = NULL;
    if (pwin->options.shown && (!pwin->options.covered)) wnputbor (pwin);
    return;
}

