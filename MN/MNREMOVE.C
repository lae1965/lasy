/*****************************************************************
    mnremove - ??????? ???? ? ??࠭?

bool mnremove (MENU *pmenu);

MENU    *pmenu;     ?????⥫? ?? ????????? ???? MENU

??室:  YES         ???? 㤠????
        NO          ?訡??
*******************************************************************/

#include <slmenu.h>

bool mnremove (MENU *pmenu) {
    MNITEM *p = pmenu->pcur;

    if (!wnremove (pmenu->pwin))  return (NO);
    pmenu->pcur = NULL;
    mnitmatr (pmenu, p);
    pmenu->pcur = p;
    if (pmenu == Cur_Menu) Cur_Menu = NULL;
    return (YES);
}

