/***************************************************************
    mndstroy - Удаляет меню с экрана и разрушает MENU

void mndstroy (MENU *pmenu);

MENU    *pmenu      Указатель на структуру меню MENU

*****************************************************************/

#include <slmenu.h>

void mndstroy (MENU *pmenu) {

    MNITEM *p, *q;

    if (!mnvalmnu (pmenu))              return;
    if (pmenu->pwin->options.shown)   wnremove (pmenu->pwin);
    wndstroy (pmenu->pwin);

    if ((p = pmenu->pitems) != NULL) {
        do { q = p->next; free (p); p = q; }
        while (q != pmenu->pitems);
    }
    if (pmenu == Cur_Menu)  Cur_Menu = NULL;
    pmenu->sign[0] = EOS;
    free (pmenu);
    return;
}

